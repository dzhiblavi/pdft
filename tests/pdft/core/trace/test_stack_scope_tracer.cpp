#include "pdft/core/trace/stack_scope_tracer.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::core::trace {

TEST_CASE("testStackScopeTracerCallsCallback", "") {
    StackScopeTracerPool pool;
    bool called = false;

    {
        auto _ = pool.getTracer([&](auto...) {
            called = true;
        });
    }

    CHECK(called);
    CHECK(pool.numAllocations() == 1);
}

TEST_CASE("testStackScopeTracerChildCallsCallback", "") {
    StackScopeTracerPool pool;
    bool called = false;
    bool child_called = false;

    {
        auto t = pool.getTracer([&](auto...) {
            called = true;
        });

        CHECK(pool.numAllocations() == 1);

        {
            auto _ = t.addChild([&](auto&&...) {
                child_called = true;
            });
        }

        CHECK(pool.numAllocations() == 2);
        CHECK(child_called);
        CHECK(!called);
    }

    CHECK(called);
    CHECK(pool.numAllocations() == 2);
}

TEST_CASE("testStackScopeTracerReusedOne", "") {
    StackScopeTracerPool pool;

    for (int i = 0; i < 10; ++i) {
        auto _ = pool.getTracer();
    }

    CHECK(pool.numAllocations() == 1);
}

TEST_CASE("testStackScopeTracerReusedWithChildren", "") {
    StackScopeTracerPool pool;

    for (int i = 0; i < 100; ++i) {
        auto t = pool.getTracer();

        { auto _ = t.addChild(); }
        {
            auto c = t.addChild();
            { auto _ = c.addChild(); }
        }
        {
            auto c = t.addChild();
            { auto _ = c.addChild(); }
        }
        CHECK(pool.numAllocations() == 6);
    }

    CHECK(pool.numAllocations() == 6);
}

TEST_CASE("testStackScopeTracerChlidrenNotYetRecycled", "") {
    StackScopeTracerPool pool;

    auto t = pool.getTracer();
    CHECK(pool.numAllocations() == 1);

    for (int i = 0; i < 10; ++i) {
        auto _ = t.addChild();
    }

    CHECK(pool.numAllocations() == 11);
}

}  // namespace pdft::core::trace
