#pragma once

#include "pdft/core/time_traits.h"
#include "pdft/utils/lock_free_stack.h"

#include <source_location>

namespace pdft::trace {

class StackScopeTracerPool;

struct StackScopeTracerBase : utils::IntrusiveForwardListItem<StackScopeTracerBase> {
    void finalize();
    void release();
    void addChild(StackScopeTracerBase* child);

    SteadyTimePointType started_at{};
    SteadyDurationType duration{};
    std::source_location loc;
    const char* label = "";

    // Stack info
    uint32_t depth = 0;
    StackScopeTracerBase* parent = nullptr;
    utils::MPMCLockFreeStack<StackScopeTracerBase> children;

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
    StackScopeTracerOwner(StackScopeTracerOwner&& rhs) noexcept;

    StackScopeTracerOwner& operator=(StackScopeTracerOwner&&) = delete;
    StackScopeTracerOwner(const StackScopeTracerOwner&) = delete;
    StackScopeTracerOwner& operator=(const StackScopeTracerOwner&) = delete;

    StackScopeTracerOwner addChild(
        const char* label = "", std::source_location loc = std::source_location::current());

    const StackScopeTracerBase& getInfo() const;

    template <typename T>
    decltype(auto) operator|(T&& t) && {
        return std::forward<T>(t);
    }

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

    static StackScopeTracerPool& globalInstance();

 private:
    StackScopeTracerBase* getTracerBase();
    void release(StackScopeTracerBase* item);

    utils::MPMCLockFreeStack<StackScopeTracerBase> free_list_;

    friend class StackScopeTraceHandle;
    friend struct StackScopeTracerBase;

    size_t num_allocations_ = 0;
};

StackScopeTracerOwner makeTracer(
    const char* label = "", std::source_location loc = std::source_location::current());

}  // namespace pdft::trace
