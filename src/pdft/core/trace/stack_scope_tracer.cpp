#include "pdft/core/trace/stack_scope_tracer.h"

namespace pdft::core::trace {

void StackScopeTracerBase::onExit() {
    duration = ClockType::now() - started_at;
}

void StackScopeTracerBase::release() {
    pool->release(this);
}

void StackScopeTracerBase::addChild(StackScopeTracerBase* child) {
    ++child->refcount;
    children.pushFront(*child);
}

StackScopeTracerBase* StackScopeTracerPool::getTracerBase() {
    if (free_list_.isEmpty()) {
        ++num_allocations_;
        return new StackScopeTracerBase;
    }

    return &free_list_.popFront();
}

void StackScopeTracerPool::release(StackScopeTracerBase* item) {
    PDFT_VERIFY(item != nullptr);
    PDFT_VERIFY(item->refcount > 0);

    --item->refcount;

    if (item->refcount == 0) {
        item->children.forEach(&StackScopeTracerBase::release);
        free_list_.pushFront(*item);
    }
}

StackScopeTracerPool& getGlobalPool() {
    static StackScopeTracerPool pool;
    return pool;
}

}  // namespace pdft::core::trace
