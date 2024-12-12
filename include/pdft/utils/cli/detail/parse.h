#pragma once

#include "pdft/utils/cli/cli.h"
#include "pdft/utils/cli/detail/cli_arg.h"

#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/unit.h"

#include <tclap/CmdLine.h>

#include <rfl/Field.hpp>
#include <rfl/apply.hpp>
#include <rfl/to_view.hpp>

#include <span>

namespace pdft::cli::detail {

template <typename S>
Result<S, std::string> parseFromCliArgs(std::span<const char*> args) {
    TCLAP::CmdLine cmd("", ' ', "", false);
    cmd.ignoreUnmatched(true);

    S result;
    auto view = rfl::to_view(result);
    auto fields = view.fields();

    auto make = [&]<auto Name, typename Description, typename T>(
                    rfl::Field<Name, CliOption<T, Description>*>& field) {
        auto res = std::make_unique<std::pair<CliOption<T, Description>*, CliArg<Description, T>>>(
            std::piecewise_construct, std::forward_as_tuple(field.get()), std::forward_as_tuple());

        res->second.attach(cmd);
        return res;
    };

    auto tuple =
        rfl::apply([&](auto&&... field) { return std::make_tuple(make(field)...); }, fields);

    try {
        cmd.parse(args.size(), args.data());
    } catch (const TCLAP::ArgException& e) {
        return std::unexpected(std::format("Error for argument '{}': {}", e.argId(), e.error()));
    }

    auto apply = [&]<typename Description, typename T>(
                     std::pair<CliOption<T, Description>*, CliArg<Description, T>>& pair) {
        auto& [dest, cli_arg] = pair;

        struct Consumer : IConsumer<T> {
            Consumer(CliOption<T, Description>* d) : dst(d) {}

            // IConsumer<T>
            Result<Unit, std::string> consume(T value) override {
                *dst = std::move(value);
                return unit;
            }

            CliOption<T, Description>* dst;
        };

        Consumer c(dest);
        return cli_arg.apply(&c);
    };

    auto do_apply = [&]<typename Self>(
                        this Self&& self, auto& arg, auto&... args) -> Result<Unit, std::string> {
        if (auto ok = apply(*arg); !ok) {
            return std::unexpected(std::move(ok.error()));
        }

        if constexpr (sizeof...(args) != 0) {
            return self(args...);
        } else {
            return unit;
        }
    };

    return std::apply(do_apply, tuple).transform([&](Unit) { return result; });
}

}  // namespace pdft::cli::detail
