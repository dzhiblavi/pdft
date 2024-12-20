#pragma once

#include "pdft/utils/fixed_string.h"

namespace pdft::rt::detail {

template <utils::FixedString A, utils::FixedString D>
constexpr auto orEmpty() {
    if constexpr (A.empty()) {
        return D;
    } else {
        return A;
    }
}

template <char A, char B>
constexpr char orEmpty() {
    return A == '\0' ? B : A;
}

// state_mode -> state-mode
template <utils::FixedString A>
constexpr auto kebabify() {
    auto arr = A.arr_;
    std::replace(arr.begin(), arr.end(), '_', '-');
    return utils::FixedString(arr);
}

template <char TShort, utils::FixedString TDescription, utils::FixedString TLong>
struct OptionDescription {
    static constexpr auto Short = TShort;
    static constexpr auto Long = TLong;
    static constexpr auto Description = TDescription;

    template <char NewShort>
    using shrt = OptionDescription<NewShort, Description, Long>;

    template <utils::FixedString NewLong>
    using lng = OptionDescription<Short, Description, NewLong>;

    template <utils::FixedString NewDescription>
    using desc = OptionDescription<Short, NewDescription, Long>;

    template <utils::FixedString Name>
    using fulfill = OptionDescription<
        orEmpty<Short, Name[0]>(),
        Description,
        orEmpty<Long, kebabify<Name>()>()>;
};

template <typename T, typename Desc>
struct CliOption {
 public:
    using Description = Desc;

    template <char NewShort>
    using shrt = CliOption<T, typename Desc::template shrt<NewShort>>;

    template <utils::FixedString NewLong>
    using lng = CliOption<T, typename Desc::template lng<NewLong>>;

    template <utils::FixedString NewDescription>
    using desc = CliOption<T, typename Desc::template desc<NewDescription>>;

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

}  // namespace pdft::cli::detail
