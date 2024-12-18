#include "pdft/core/cli/cli.h"
#include "pdft/core/cli/storage.h"
#include "pdft/core/module/detail/make_traits.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/load.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::module {

struct NoArgumentsModule : Module {
    NoArgumentsModule() = default;
};

struct IoArgumentModule : Module {
    IoArgumentModule(asio::io_context&) {}
};

struct ConfigArgumentModule : Module {
    struct Config {
        int x;
    };

    ConfigArgumentModule(Config c) {
        CHECK(c.x == 10);
    }
};

struct CliArgsArgumentModule : Module {
    struct CliArgs {
        cli::opt<int> x;
    };

    CliArgsArgumentModule(CliArgs c) {
        CHECK(c.x.get() == 20);
    }
};

struct InjectArgumentModule : Module {
    InjectArgumentModule(inject<NoArgumentsModule, "no-args">) {}
};

struct MixedArgumentsModule : Module {
    struct Config {
        int x;
    };
    struct CliArgs {
        cli::opt<int> x;
    };

    MixedArgumentsModule(
        Config,
        CliArgs,
        asio::io_context&,
        inject<NoArgumentsModule, "no-args">,
        inject<IoArgumentModule, "io">) {}
};

TEST_CASE("LoadContext/getConfig/does-not-exist") {
    asio::io_context io;
    FactoryStorage storage;

    LoadContext ctx(io, {}, storage);
    CHECK(!ctx.getConfig("test"));
}

TEST_CASE("LoadContext/getConfig/exists") {
    asio::io_context io;
    FactoryStorage storage;

    LoadContext ctx(io, {{"test", {}}}, storage);
    CHECK(ctx.getConfig("test").has_value());
}

TEST_CASE("LoadContext/getModule/no-args") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-factory", detail::moduleTraits<NoArgumentsModule>());
    ModuleConfig module{.cls = "test-factory"};
    LoadContext ctx(io, {{"test", module}}, storage);

    auto mod = ctx.getModule<NoArgumentsModule>("test");
    CHECK(mod.has_value());
}

TEST_CASE("LoadContext/getModule/io-arg") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-factory", detail::moduleTraits<IoArgumentModule>());
    ModuleConfig module{.cls = "test-factory"};
    LoadContext ctx(io, {{"test", module}}, storage);

    auto mod = ctx.getModule<IoArgumentModule>("test");
    CHECK(mod.has_value());
}

TEST_CASE("LoadContext/getModule/config-arg") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-factory", detail::moduleTraits<ConfigArgumentModule>());

    ModuleConfig module{
        .cls = "test-factory",
        .config = rfl::to_generic(ConfigArgumentModule::Config{.x = 10}),
    };
    LoadContext ctx(io, {{"test", module}}, storage);

    auto mod = ctx.getModule<ConfigArgumentModule>("test");
    CHECK(mod.has_value());
}

// TEST_CASE("LoadContext/getModule/cli-arg") {
// asio::io_context io;
// FactoryStorage storage;
// cli::OptionsStorage cli_options;

// storage.registerModule("test-factory", detail::moduleTraits<CliArgsArgumentModule>());

// ModuleConfig module{.cls = "test-factory"};
// LoadContext ctx(io, {{"test", module}}, storage);

// auto mod = ctx.getModule<CliArgsArgumentModule>("test");
// CHECK(mod.has_value());
//}

}  // namespace pdft::module
