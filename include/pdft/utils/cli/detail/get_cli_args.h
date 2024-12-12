#pragma once

#include "pdft/utils/cli/cli.h"
#include "pdft/utils/cli/detail/cli_arg.h"

#include <rfl/Field.hpp>
#include <rfl/apply.hpp>
#include <rfl/to_view.hpp>

namespace pdft::cli::detail {

// XXX: Usage of internal namespace
template <size_t N>
constexpr utils::FixedString<N> fixedStringFromStringLiteral(
    rfl::internal::StringLiteral<N> literal) {
    return {literal.arr_};
}

template <auto Name, typename D, typename T>
auto makeCliArgForField(rfl::Field<Name, CliOption<T, D>*>*) {
    using Desc = typename D::template UpdateLongIfEmpty<fixedStringFromStringLiteral(Name)>;
    return std::make_unique<CliArg<Desc, T>>();
}

// tuple<unique_ptr<CliArg<T, D>>...>&
template <typename C>
auto makeCliArgs() {
    C result;
    auto view = rfl::to_view(result);
    auto fields = view.fields();

    return rfl::apply(
        [&](auto&&... field) { return std::make_tuple(makeCliArgForField(&field)...); }, fields);
}

template <auto Name, typename D, typename T>
auto makeConsumerForField(rfl::Field<Name, CliOption<T, D>*>& field) {
    struct Consumer : detail::IConsumer<T> {
        Consumer(CliOption<T, D>* d) : dst(d) {}

        // IConsumer<T>
        Result<Unit, std::string> consume(T value) override {
            *dst = std::move(value);
            return unit;
        }

        CliOption<T, D>* dst;
    };

    return Consumer(field.get());
}

// tuple<IConsumer<T> auto>...>
template <typename C>
auto makeConsumers(C& instance) {
    auto view = rfl::to_view(instance);
    auto fields = view.fields();

    return rfl::apply(
        [&](auto&&... field) { return std::make_tuple(makeConsumerForField(field)...); }, fields);
}

// tuple<unique_ptr<CliArg<T, D>>...>&
template <typename C>
static auto& getCliArgs() {
    static auto args = makeCliArgs<C>();
    return args;
}

}  // namespace pdft::cli::detail
