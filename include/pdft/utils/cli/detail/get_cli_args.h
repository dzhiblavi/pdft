#pragma once

#include "pdft/utils/cli/cli.h"
#include "pdft/utils/cli/detail/cli_arg.h"

#include <rfl/Field.hpp>
#include <rfl/apply.hpp>
#include <rfl/to_view.hpp>

namespace pdft::cli::detail {

// tuple<unique_ptr<CliArg<T, D>>...>&
template <typename C>
auto makeCliArgs() {
    C result;
    auto view = rfl::to_view(result);
    auto fields = view.fields();

    auto make = [&]<auto Name, typename Description, typename T>(
                    rfl::Field<Name, CliOption<T, Description>*>& /*field*/) {
        return std::make_unique<CliArg<Description, T>>();
    };

    return rfl::apply([&](auto&&... field) { return std::make_tuple(make(field)...); }, fields);
}

// tuple<IConsumer<T> auto>...>
template <typename C>
auto makeConsumers(C& instance) {
    auto view = rfl::to_view(instance);
    auto fields = view.fields();

    auto make = [&]<auto Name, typename Description, typename T>(
                    rfl::Field<Name, CliOption<T, Description>*>& field) {
        struct Consumer : detail::IConsumer<T> {
            Consumer(CliOption<T, Description>* d) : dst(d) {}

            // IConsumer<T>
            Result<Unit, std::string> consume(T value) override {
                *dst = std::move(value);
                return unit;
            }

            CliOption<T, Description>* dst;
        };

        return Consumer(field.get());
    };

    return rfl::apply([&](auto&&... field) { return std::make_tuple(make(field)...); }, fields);
}

// tuple<unique_ptr<CliArg<T, D>>...>&
template <typename C>
static auto& getCliArgs() {
    static auto args = makeCliArgs<C>();
    return args;
}

}  // namespace pdft::cli::detail
