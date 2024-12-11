#pragma once

#include "pdft/core/result/type/unit.h"
#include "pdft/utils/instanceof_template.h"

#include <expected>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include <tclap/CmdLine.h>

#include <rfl/Field.hpp>
#include <rfl/apply.hpp>
#include <rfl/to_view.hpp>

#include <magic_enum.hpp>

namespace pdft::utils {

template <typename T>
struct IConsumer {
    virtual ~IConsumer() = default;
    virtual std::expected<Unit, std::string> consume(T value) = 0;
};

template <typename T>
struct ICliArg {
    virtual ~ICliArg() = default;
    virtual std::expected<Unit, std::string> apply(IConsumer<T>* consumer) = 0;
};

template <typename T>
concept BasicType = !utils::InstanceOfTemplate<T, std::optional> &&
                    !utils::InstanceOfTemplate<T, std::vector> && !std::is_enum_v<T>;

template <typename T>
struct ComplexType;

template <typename T>
struct ComplexType<std::optional<T>> {
    using type = T;

    static std::expected<std::optional<T>, std::string> lift(type value) {
        return std::optional<T>{std::move(value)};
    }
};

template <typename T>
struct ComplexType<std::vector<T>> {
    using Traits = ComplexType<T>;
    using U = typename Traits::type;
    using type = std::vector<U>;

    static std::expected<std::vector<T>, std::string> lift(type value) {
        std::vector<T> result;
        result.reserve(value.size());

        for (auto&& elem : value) {
            auto lifted = Traits::lift(std::move(elem));

            if (!lifted) {
                return std::unexpected(lifted.error());
            }

            result.push_back(*lifted);
        }

        return result;
    }
};

template <typename T>
requires std::is_enum_v<T>
struct ComplexType<T> {
    using type = std::string;

    static std::expected<T, std::string> lift(type value) {
        auto e = magic_enum::enum_cast<T>(value);

        if (!e.has_value()) {
            return std::unexpected(std::format("failed to parse enum from {}", value));
        }

        return *e;
    }
};

// Specialization for arbitrarily complex types
template <auto Name, typename T>
struct CliArg : ICliArg<T>, IConsumer<typename ComplexType<T>::type> {
    using Traits = ComplexType<T>;
    using U = typename Traits::type;

    CliArg(TCLAP::CmdLine& line) : impl_(line) {}

    // IConsumer<U>
    std::expected<Unit, std::string> consume(U value) override {
        auto lifted = Traits::lift(std::move(value));

        if (!lifted) {
            return std::unexpected(lifted.error());
        }

        return consumer_->consume(*lifted);
    }

    // ICliArg<T>
    std::expected<Unit, std::string> apply(IConsumer<T>* consumer) override {
        consumer_ = consumer;
        return impl_.apply(this);
    }

 private:
    IConsumer<T>* consumer_ = nullptr;
    CliArg<Name, U> impl_;
};

// Regular types (int, string, float, ...)
template <auto Name, BasicType T>
struct CliArg<Name, T> : ICliArg<T> {
    CliArg(TCLAP::CmdLine& line)
        : arg_(Name.str().substr(0, 1), Name.str(), "", false, T{}, "", line) {}

    // ICliArg<T>
    std::expected<Unit, std::string> apply(IConsumer<T>* consumer) override {
        if (!arg_.isSet()) {
            return unit;
        }

        return consumer->consume(arg_.getValue());
    }

 private:
    TCLAP::ValueArg<T> arg_;
};

// Boolean types are just switches
template <auto Name>
struct CliArg<Name, bool> : ICliArg<bool> {
    CliArg(TCLAP::CmdLine& line) : arg_(Name.str().substr(0, 1), Name.str(), "", line) {}

    // ICliArg<bool>
    std::expected<Unit, std::string> apply(IConsumer<bool>* consumer) override {
        if (!arg_.isSet()) {
            return unit;
        }

        return consumer->consume(arg_.getValue());
    }

 private:
    TCLAP::SwitchArg arg_;
};

// Specialization for vectors of basic types
template <auto Name, BasicType T>
struct CliArg<Name, std::vector<T>> : ICliArg<std::vector<T>> {
    CliArg(TCLAP::CmdLine& line) : arg_(Name.str().substr(0, 1), Name.str(), "", false, "", line) {}

    // ICliArg
    std::expected<Unit, std::string> apply(IConsumer<std::vector<T>>* consumer) override {
        return consumer->consume(arg_.getValue());
    }

 private:
    TCLAP::MultiArg<T> arg_;
};

template <typename S>
std::expected<S, std::string> parseFromCliArgs(std::span<const char*> args) {
    TCLAP::CmdLine cmd("", ' ', "", false);
    cmd.ignoreUnmatched(true);

    S result;
    auto view = rfl::to_view(result);
    auto fields = view.fields();

    auto make = [&]<auto Name, typename T>(rfl::Field<Name, T*>& field) {
        return std::make_unique<std::pair<T*, CliArg<Name, T>>>(
            std::piecewise_construct,
            std::forward_as_tuple(field.get()),
            std::forward_as_tuple(cmd));
    };

    auto tuple =
        rfl::apply([&](auto&&... field) { return std::make_tuple(make(field)...); }, fields);

    try {
        cmd.parse(args.size(), args.data());
    } catch (const TCLAP::ArgException& e) {
        return std::unexpected(std::format("Error for argument '{}': {}", e.argId(), e.error()));
    }

    auto apply = [&](auto& pair) {
        auto& [dest, cli_arg] = *pair;
        using T = std::remove_pointer_t<std::decay_t<decltype(dest)>>;

        struct Consumer : IConsumer<T> {
            Consumer(T* d) : dst(d) {}

            // IConsumer<T>
            std::expected<Unit, std::string> consume(T value) override {
                *dst = std::move(value);
                return unit;
            }

            T* dst;
        };

        Consumer c(dest);
        return cli_arg.apply(&c);
    };

    auto do_apply =
        [&]<typename Self>(
            this Self&& self, auto& arg, auto&... args) -> std::expected<Unit, std::string> {
        if (auto ok = apply(arg); !ok) {
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

}  // namespace pdft::utils
