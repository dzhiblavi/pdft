#pragma once

#include <system_error>
#include <type_traits>

#include <magic_enum.hpp>

namespace pdft {

using Error = std::error_code;

template <typename T>
requires std::is_scoped_enum_v<T>
const std::error_category& enumErrorCodeErrorCategory() {
    class Category : public std::error_category {
     public:
        const char* name() const noexcept override {
            return "hello";
        }

        std::string message(std::underlying_type_t<T> value) const override {
            return std::string(magic_enum::enum_name(static_cast<T>(value)));
        }
    };

    static const Category category;
    return category;
}

template <typename T>
requires std::is_scoped_enum_v<T>
Error makeErrorCode(T code) {
    using Underlying = std::underlying_type_t<T>;
    return Error(static_cast<Underlying>(code), enumErrorCodeErrorCategory<T>());
}

}  // namespace pdft
