#include "pdft/core/trace/stack_scope_tracer.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::core::trace {

TEST_CASE("testStackScopeTracerOneAllocation", "") {
    StackScopeTracerPool pool;

    auto t = pool.getTracer();
    t.finalize();
    CHECK(pool.numAllocations() == 1);
}

TEST_CASE("testStackScopeTracerChildCallsCallback", "") {
    StackScopeTracerPool pool;

    {
        auto t = pool.getTracer();
        CHECK(pool.numAllocations() == 1);

        { auto c = t.addChild(); }

        CHECK(pool.numAllocations() == 2);
        CHECK(t.getInfo().children.size() == 1);
    }

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
