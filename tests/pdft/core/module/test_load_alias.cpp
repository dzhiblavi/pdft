#include "pdft/core/module/detail/make_traits.h"
#include "pdft/core/module/load.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::module {

struct X : Module {
    X() = default;
};

TEST_CASE("LoadContext/load/alias/does-not-exist") {
    asio::io_context io;
    FactoryStorage storage;

    LoadContext ctx(io, {{"test", {.alias = "hello"}}}, storage);
    CHECK(!ctx.getModule("test"));
}

TEST_CASE("LoadContext/load/alias/success") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-module", detail::moduleTraits<X>());

    LoadContext ctx(
        io,
        {
            {
                "mod",
                {.cls = "test-module"},
            },
            {
                "test",
                {.alias = "mod"},
            },
        },
        storage);

    auto mod = ctx.getModule("mod");
    auto test = ctx.getModule("test");
    CHECK(mod->get() == test->get());
}

TEST_CASE("LoadContext/load/alias/transitive") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-module", detail::moduleTraits<X>());

    LoadContext ctx(
        io,
        {
            {
                "mod2",
                {.cls = "test-module"},
            },
            {
                "mod1",
                {.alias = "mod2"},
            },
            {
                "test",
                {.alias = "mod1"},
            },
        },
        storage);

    auto mod1 = ctx.getModule("mod1");
    auto mod2 = ctx.getModule("mod2");
    auto test = ctx.getModule("test");
    CHECK(mod1->get() == mod2->get());
    CHECK(mod1->get() == test->get());
}

}  // namespace pdft::module
