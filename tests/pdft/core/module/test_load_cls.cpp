#include "pdft/core/cli/cli.h"

#include "pdft/core/module/detail/make_traits.h"
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
        cli::opt<int> x = 1;
    };

    CliArgsArgumentModule(CliArgs c) {
        CHECK(c.x.get() == 1);
    }
};

struct InjectArgumentModule : Module {
    InjectArgumentModule(Arc<NoArgumentsModule>) {}
};

struct MixedArgumentsModule : Module {
    struct Config {
        int x;
    };
    struct CliArgs {
        cli::opt<int> x = 1;
    };

    MixedArgumentsModule(
        Config conf,
        CliArgs cli,
        asio::io_context&,
        Arc<NoArgumentsModule>,
        Arc<IoArgumentModule>) {
        CHECK(conf.x == 10);
        CHECK(cli.x.get() == 1);
    }
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

TEST_CASE("LoadContext/load/cls/no-args") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-factory", detail::moduleTraits<NoArgumentsModule>());
    ModuleConfig module{.cls = "test-factory"};
    LoadContext ctx(io, {{"test", module}}, storage);

    auto mod = ctx.getModule<NoArgumentsModule>("test");
    CHECK(mod.has_value());
}

TEST_CASE("LoadContext/load/cls/io-arg") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-factory", detail::moduleTraits<IoArgumentModule>());
    ModuleConfig module{.cls = "test-factory"};
    LoadContext ctx(io, {{"test", module}}, storage);

    auto mod = ctx.getModule<IoArgumentModule>("test");
    CHECK(mod.has_value());
}

TEST_CASE("LoadContext/load/cls/config-arg") {
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

TEST_CASE("LoadContext/load/cls/cli-arg") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-factory", detail::moduleTraits<CliArgsArgumentModule>());

    ModuleConfig module{.cls = "test-factory"};
    LoadContext ctx(io, {{"test", module}}, storage);

    auto mod = ctx.getModule<CliArgsArgumentModule>("test");
    CHECK(mod.has_value());
}

TEST_CASE("LoadContext/load/cls/inject-arg") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-noargs", detail::moduleTraits<NoArgumentsModule>());
    storage.registerModule("test-factory", detail::moduleTraits<InjectArgumentModule>());

    ModuleConfig module{.cls = "test-factory"};
    LoadContext ctx(
        io,
        {
            {
                "test",
                {
                    .cls = "test-factory",
                    .deps = {"dependency"},
                },
            },
            {
                "dependency",
                {.cls = "test-noargs"},
            },
        },
        storage);

    auto mod = ctx.getModule<InjectArgumentModule>("test");
    CHECK(mod.has_value());
}

TEST_CASE("LoadContext/load/cls/mixed-args") {
    asio::io_context io;
    FactoryStorage storage;

    storage.registerModule("test-noargs", detail::moduleTraits<NoArgumentsModule>());
    storage.registerModule("test-io", detail::moduleTraits<IoArgumentModule>());
    storage.registerModule("test-factory", detail::moduleTraits<MixedArgumentsModule>());

    LoadContext ctx(
        io,
        {
            {
                "test",
                {
                    .cls = "test-factory",
                    .config = rfl::to_generic(MixedArgumentsModule::Config{.x = 10}),
                    .deps =
                        {
                            // The limitation of the current load system is that
                            // one should fix all dependencies here, including non-module ones,
                            // except when there are no non-module deps.
                            "",
                            "",
                            "",
                            // shared module
                            "dep-no-args",
                            // anonymous module
                            ModuleConfig{
                                .cls = "test-io",
                            },
                        },
                },
            },
            {
                "dep-no-args",
                {.cls = "test-noargs"},
            },
        },
        storage);

    auto mod = ctx.getModule<MixedArgumentsModule>("test");
    log::info(mod.error());
    CHECK(mod.has_value());
}

}  // namespace pdft::module
