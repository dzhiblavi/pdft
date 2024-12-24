#include "pdft/core/trace/stack_scope_tracer.h"

#include "pdft/core/assert.h"

namespace pdft::trace {

void StackScopeTracerBase::finalize() {
    duration = SteadyClockType::now() - started_at;
}

void StackScopeTracerBase::release() {
    pool->release(this);
}

void StackScopeTracerBase::addChild(StackScopeTracerBase* child) {
    ++child->refcount;
    children.push(*child);
}

StackScopeTraceHandle::StackScopeTraceHandle(const StackScopeTracerOwner& owner)
    : StackScopeTraceHandle(owner.base_) {}

bool StackScopeTraceHandle::isValid() const {
    return base_ != nullptr;
}

StackScopeTracerOwner StackScopeTraceHandle::addChild(const char* label, std::source_location loc) {
    verify{isValid()};
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

StackScopeTracerOwner::StackScopeTracerOwner(StackScopeTracerOwner&& rhs) noexcept
    : base_(std::exchange(rhs.base_, nullptr)) {}

StackScopeTracerOwner StackScopeTracerOwner::addChild(const char* label, std::source_location loc) {
    return StackScopeTraceHandle{verify(base_)}.addChild(label, loc);
}

const StackScopeTracerBase& StackScopeTracerOwner::getInfo() const {
    return *verify(base_);
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
    if (auto base = free_list_.tryPop()) {
        return base;
    }

    ++num_allocations_;
    return new StackScopeTracerBase;
}

StackScopeTracerPool::~StackScopeTracerPool() {
    size_t num_freed = 0;

    free_list_.forEach([&](auto&& item) {
        ++num_freed;
        delete &item;
    });

    PDFT_VERIFY_DBG(num_freed == numAllocations());
}

StackScopeTracerOwner StackScopeTracerPool::getTracer(const char* label, std::source_location loc) {
    auto* base = getTracerBase();

    base->started_at = SteadyClockType::now();
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
    verify(item != nullptr);
    verify(item->refcount > 0);

    --item->refcount;

    if (item->refcount == 0) {
        item->children.forEach(&StackScopeTracerBase::release);
        free_list_.push(*item);
    }
}

StackScopeTracerPool& StackScopeTracerPool::globalInstance() {
    static StackScopeTracerPool pool;
    return pool;
}

StackScopeTracerOwner makeTracer(const char* label, std::source_location loc) {
    return StackScopeTracerPool::globalInstance().getTracer(label, loc);
}

}  // namespace pdft::trace
