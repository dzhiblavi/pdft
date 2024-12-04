#pragma once

#include "pdft/core/trace/time_traits.h"
#include "pdft/utils/intrusive_forward_list.h"

#include <source_location>

namespace pdft::trace {

class StackScopeTracerPool;

struct StackScopeTracerBase : utils::IntrusiveForwardListItem<StackScopeTracerBase> {
    void finalize();
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

class StackScopeTracerOwner;

class StackScopeTraceHandle {
 public:
    StackScopeTraceHandle() = default;
    StackScopeTraceHandle(const StackScopeTracerOwner& owner);
    StackScopeTraceHandle(const StackScopeTraceHandle&) = default;
    StackScopeTraceHandle& operator=(const StackScopeTraceHandle&) = default;

    StackScopeTracerOwner addChild(
        const char* label = "", std::source_location loc = std::source_location::current());

    bool isValid() const;

 private:
    explicit StackScopeTraceHandle(StackScopeTracerBase* base) : base_(base) {}

    StackScopeTracerBase* base_ = nullptr;

    friend class StackScopeTracerOwner;
};

class StackScopeTracerOwner {
 public:
    ~StackScopeTracerOwner();
    StackScopeTracerOwner() = default;
    StackScopeTracerOwner(StackScopeTracerOwner&& rhs);

    StackScopeTracerOwner& operator=(StackScopeTracerOwner&&) = delete;
    StackScopeTracerOwner(const StackScopeTracerOwner&) = delete;
    StackScopeTracerOwner& operator=(const StackScopeTracerOwner&) = delete;

    StackScopeTracerOwner addChild(
        const char* label = "", std::source_location loc = std::source_location::current());

    const StackScopeTracerBase& getInfo() const;

    void finalize();

 private:
    explicit StackScopeTracerOwner(StackScopeTracerBase* base) : base_(base) {}

    void release();

    StackScopeTracerBase* base_ = nullptr;

    friend class StackScopeTraceHandle;
    friend class StackScopeTracerPool;

    template <typename C>
    friend class StackScopeTracer;
};

class StackScopeTracerPool {
 public:
    StackScopeTracerPool() = default;

    ~StackScopeTracerPool();

    StackScopeTracerOwner getTracer(
        const char* label = "", std::source_location loc = std::source_location::current());

    size_t numAllocations() const;

 private:
    StackScopeTracerBase* getTracerBase();
    void release(StackScopeTracerBase* item);

    utils::IntrusiveForwardList<StackScopeTracerBase> free_list_;

    friend class StackScopeTraceHandle;
    friend struct StackScopeTracerBase;

    size_t num_allocations_ = 0;
};

StackScopeTracerPool& getGlobalPool();

}  // namespace pdft::trace
