#include "pdft/core/module/factory_storage.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::module {

struct X {};

ModuleTraits* getModuleTraits() {
    struct XTraits : ModuleTraits {
        Result<Arc<Module>> createVia(
            LoadContext&, const ModuleConfig&, const std::string&) const override {
            return nullptr;
        }

        rfl::Generic getConfig() const override {
            return rfl::Generic::Null;
        }
    };

    static XTraits traits;
    return &traits;
}

FactoryTraits* getFactoryTraits() {
    struct XTraits : FactoryTraits {
        Result<Arc<Module>> create(Module*) const override {
            return nullptr;
        }
    };

    static XTraits traits;
    return &traits;
}

TEST_CASE("FactoryStorage/register/module") {
    FactoryStorage storage;

    auto* traits = getModuleTraits();
    storage.registerModule("test", traits);

    CHECK(*storage.getModuleTraits("test") == traits);
}

TEST_CASE("FactoryStorage/register/factory") {
    FactoryStorage storage;

    auto* mod = getModuleTraits();
    auto* fac = getFactoryTraits();
    storage.registerFactoryModule("test", mod, fac);

    CHECK(*storage.getModuleTraits("test") == mod);
    CHECK(*storage.getFactoryTraits("test") == fac);
}

TEST_CASE("FactoryStorage/register/module/duplicate") {
    FactoryStorage storage;

    auto* traits = getModuleTraits();
    storage.registerModule("test", traits);
    CHECK(!storage.registerModule("test", traits));
}

TEST_CASE("FactoryStorage/register/factory/duplicate") {
    FactoryStorage storage;

    auto* mod = getModuleTraits();
    auto* fac = getFactoryTraits();
    storage.registerFactoryModule("test", mod, fac);
    CHECK(!storage.registerFactoryModule("test", mod, fac));
}

}  // namespace pdft::module
