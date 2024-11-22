#pragma once

#include "pdft/core/trace/time_traits.h"
#include "pdft/utils/intrusive_forward_list.h"

#include <source_location>

namespace pdft::core::trace {

class StackScopeTracerPool;

struct Noop {
    void operator()(auto&&...) {}
};

struct StackScopeTracerBase : utils::IntrusiveForwardListItem<StackScopeTracerBase> {
    void onExit();
    void release();
    void addChild(StackScopeTracerBase* child);

    TimePointType started_at;
    DurationType duration;
    std::source_location loc;
    const char* label = "";

    // Stack info
    uint32_t depth = 0;
    StackScopeTracerBase* parent = nullptr;
    utils::IntrusiveForwardList<StackScopeTracerBase> children;

    // Reference count before returning to the pool
    uint32_t refcount = 0;
    StackScopeTracerPool* pool = nullptr;
};

template <typename Callback>
class StackScopeTracer {
 public:
    ~StackScopeTracer() {
        manualExit();
    }

    StackScopeTracer(StackScopeTracer&& rhs)
        : base_(std::exchange(rhs.base_, nullptr))
        , callback_(std::move(rhs.callback_)) {}

    StackScopeTracer& operator=(StackScopeTracer&&) = delete;
    StackScopeTracer(const StackScopeTracer&) = delete;
    StackScopeTracer& operator=(const StackScopeTracer&) = delete;

    template <typename ChildCallback>
    StackScopeTracer<ChildCallback> addChild(
        ChildCallback&& callback,
        const char* label = "",
        std::source_location loc = std::source_location::current());

    StackScopeTracer<Noop> addChild(
        const char* label = "", std::source_location loc = std::source_location::current()) {
        return addChild(Noop(), label, loc);
    }

    void manualExit() {
        if (base_ == nullptr) {
            return;
        }

        base_->onExit();
        std::invoke(std::move(callback_), *base_);
        base_->release();
        base_ = nullptr;
    }

 private:
    StackScopeTracer(StackScopeTracerBase* base, Callback callback)
        : base_(base)
        , callback_(std::move(callback)) {}

    StackScopeTracerBase* base_;
    [[no_unique_address]] Callback callback_;

    friend class StackScopeTracerPool;

    template <typename C>
    friend class StackScopeTracer;
};

class StackScopeTracerPool {
 public:
    StackScopeTracerPool() = default;

    ~StackScopeTracerPool() {
        size_t total_freed = 0;

        free_list_.forEach([&](auto& item) {
            ++total_freed;
            delete &item;
        });

        PDFT_VERIFY(total_freed == numAllocations());
    }

    template <typename Callback>
    StackScopeTracer<Callback> getTracer(
        Callback&& callback,
        const char* label = "",
        std::source_location loc = std::source_location::current()) {
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

        return {base, std::forward<Callback>(callback)};
    }

    StackScopeTracer<Noop> getTracer(
        const char* label = "", std::source_location loc = std::source_location::current()) {
        return getTracer(Noop(), label, loc);
    }

    size_t numAllocations() const {
        return num_allocations_;
    }

 private:
    StackScopeTracerBase* getTracerBase();
    void release(StackScopeTracerBase* item);

    utils::IntrusiveForwardList<StackScopeTracerBase> free_list_;

    friend struct StackScopeTracerBase;

    size_t num_allocations_ = 0;
};

StackScopeTracerPool& getGlobalPool();

template <typename Callback>
template <typename ChildCallback>
StackScopeTracer<ChildCallback> StackScopeTracer<Callback>::addChild(
    ChildCallback&& callback, const char* label, std::source_location loc) {
    auto c = base_->pool->getTracer(std::forward<ChildCallback>(callback), label, loc);
    c.base_->depth = base_->depth + 1;
    c.base_->parent = base_;
    base_->addChild(c.base_);
    return c;
}

}  // namespace pdft::core::trace
