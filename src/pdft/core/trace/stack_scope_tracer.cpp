#include "pdft/core/trace/stack_scope_tracer.h"

namespace pdft::trace {

void StackScopeTracerBase::finalize() {
    duration = ClockType::now() - started_at;
}

void StackScopeTracerBase::release() {
    pool->release(this);
}

void StackScopeTracerBase::addChild(StackScopeTracerBase* child) {
    ++child->refcount;
    children.pushFront(*child);
}

StackScopeTraceHandle::StackScopeTraceHandle(const StackScopeTracerOwner& owner)
    : StackScopeTraceHandle(owner.base_) {}

bool StackScopeTraceHandle::isValid() const {
    return base_ != nullptr;
}

StackScopeTracerOwner StackScopeTraceHandle::addChild(const char* label, std::source_location loc) {
    PDFT_VERIFY(isValid());
    auto c = base_->pool->getTracer(label, loc);
    c.base_->depth = base_->depth + 1;
    c.base_->parent = base_;
    base_->addChild(c.base_);
    return c;
}

StackScopeTracerOwner::~StackScopeTracerOwner() {
    finalize();
    release();
}

StackScopeTracerOwner::StackScopeTracerOwner(StackScopeTracerOwner&& rhs)
    : base_(std::exchange(rhs.base_, nullptr)) {}

StackScopeTracerOwner StackScopeTracerOwner::addChild(const char* label, std::source_location loc) {
    return StackScopeTraceHandle{PDFT_VERIFY(base_)}.addChild(label, loc);
}

const StackScopeTracerBase& StackScopeTracerOwner::getInfo() const {
    return *PDFT_VERIFY(base_);
}

void StackScopeTracerOwner::finalize() {
    if (base_ == nullptr) {
        return;
    }

    base_->finalize();
}

void StackScopeTracerOwner::release() {
    if (base_ == nullptr) {
        return;
    }

    base_->release();
    base_ = nullptr;
}

StackScopeTracerBase* StackScopeTracerPool::getTracerBase() {
    if (free_list_.isEmpty()) {
        ++num_allocations_;
        return new StackScopeTracerBase;
    }

    return &free_list_.popFront();
}

StackScopeTracerPool::~StackScopeTracerPool() {
    PDFT_VERIFY(free_list_.size() == numAllocations());
    free_list_.forEachUnordered([](auto&& item) { delete &item; });
}

StackScopeTracerOwner StackScopeTracerPool::getTracer(const char* label, std::source_location loc) {
    auto* base = getTracerBase();

    base->started_at = ClockType::now();
    base->duration = {};
    base->loc = loc;
    base->label = label;

    base->depth = 0;
    base->parent = nullptr;
    base->children.clear();

    base->refcount = 1;
    base->pool = this;

    return StackScopeTracerOwner{base};
}

size_t StackScopeTracerPool::numAllocations() const {
    return num_allocations_;
}

void StackScopeTracerPool::release(StackScopeTracerBase* item) {
    PDFT_VERIFY(item != nullptr);
    PDFT_VERIFY(item->refcount > 0);

    --item->refcount;

    if (item->refcount == 0) {
        item->children.forEachUnordered(&StackScopeTracerBase::release);
        free_list_.pushFront(*item);
    }
}

StackScopeTracerPool& getGlobalPool() {
    thread_local StackScopeTracerPool pool;
    return pool;
}

}  // namespace pdft::trace
