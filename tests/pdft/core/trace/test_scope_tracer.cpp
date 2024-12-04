#include "pdft/core/trace/scope_tracer.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::trace {

TEST_CASE("testScopeTracerCallsCallback", "") {
    bool called = false;

    {
        auto _ = ScopeTracer([&](auto...) { called = true; });
    }

    CHECK(called);
}

}  // namespace pdft::trace
