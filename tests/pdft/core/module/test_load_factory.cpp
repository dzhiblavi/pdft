#include "pdft/core/module/detail/make_traits.h"
#include "pdft/core/module/load.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::module {

struct I {
    virtual ~I() = default;
    virtual void foo() = 0;
};

struct X : I, Module {
    X(int value) {
        CHECK(value == 123);
    }

    void foo() override {}
};

struct F : AbstractFactory<I>, Module {
    Arc<I> create() const override {
        return std::make_shared<X>(123);
    }
};

TEST_CASE("LoadContext/load/factory/does-not-exist") {
    asio::io_context io;
    FactoryStorage storage;

    LoadContext ctx(io, {{"test", {.factory = "Hello"}}}, storage);
    CHECK(!ctx.getModule("test"));
}

TEST_CASE("LoadContext/load/factory/success") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerFactoryModule(
        "test-factory", detail::moduleTraits<F>(), detail::factoryTraits<I, F>());

    LoadContext ctx(
        io,
        {
            {
                "factory",
                {.cls = "test-factory"},
            },
            {
                "test",
                {.factory = "factory"},
            },
        },
        storage);

    CHECK(ctx.getModule("test"));
}

TEST_CASE("LoadContext/load/factory/factory-of-factories-not-supported") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerFactoryModule(
        "test-factory", detail::moduleTraits<F>(), detail::factoryTraits<I, F>());

    LoadContext ctx(
        io,
        {
            {
                "factory-factory",
                {.cls = "test-factory"},
            },
            {
                "factory",
                {.factory = "factory-factory"},
            },
            {
                "test",
                {.factory = "factory"},
            },
        },
        storage);

    CHECK(!ctx.getModule("test"));
}

}  // namespace pdft::module
