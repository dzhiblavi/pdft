#pragma once

#include "pdft/core/result/type/result.h"

#include "pdft/utils/instanceof_template.h"

namespace pdft::rt::detail {

template <typename T>
concept BasicType = !utils::InstanceOfTemplate<T, std::optional> &&
                    !utils::InstanceOfTemplate<T, std::vector> && !std::is_enum_v<T>;

template <typename T>
struct ComplexType;

template <typename T>
struct ComplexType<std::optional<T>> {
    using type = T;

    static Result<std::optional<T>, std::string> lift(type value) {
        return std::optional<T>{std::move(value)};
    }
};

template <typename T>
struct ComplexType<std::vector<T>> {
    using Traits = ComplexType<T>;
    using U = typename Traits::type;
    using type = std::vector<U>;

    static Result<std::vector<T>, std::string> lift(type value) {
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

    static Result<T, std::string> lift(type value) {
        auto e = magic_enum::enum_cast<T>(value);

        if (!e.has_value()) {
            return std::unexpected(
                std::format("failed to parse {} from {}", rfl::type_name_t<T>().name(), value));
        }

        return *e;
    }
};

}  // namespace pdft::rt::detail
