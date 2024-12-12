#pragma once

#include "pdft/utils/fixed_string.h"

namespace pdft::cli {

template <char TShort, utils::FixedString TLong, utils::FixedString TDescription>
struct OptionDescription {
    static constexpr auto Short = TShort;
    static constexpr auto Long = TLong;
    static constexpr auto Description = TDescription;
};

template <typename T, typename Desc>
struct CliOption {
 public:
    using Description = Desc;

    template <typename... Args>
    CliOption(Args&&... args) : value_(std::forward<Args>(args)...) {}

    CliOption& operator=(const T& value) {
        value_ = value;
        return *this;
    }

    /* implicit */ operator T() {
        return value_;
    }

    T* operator->() {
        return &value_;
    }

    const T* operator->() const {
        return &value_;
    }

    T& get() {
        return value_;
    }

    const T& get() const {
        return value_;
    }

 private:
    T value_;
};

template <typename T, char Short, utils::FixedString Long = "", utils::FixedString Description = "">
using cli = CliOption<T, OptionDescription<Short, Long, Description>>;

}  // namespace pdft::cli
