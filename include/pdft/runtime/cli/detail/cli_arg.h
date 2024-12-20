#pragma once

#include "pdft/runtime/cli/detail/lift.h"
#include "pdft/runtime/cli/option.h"

#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/unit.h"

#include <tclap/CmdLine.h>

namespace pdft::rt::detail {

template <typename T>
struct IConsumer {
    virtual ~IConsumer() = default;
    virtual Result<Unit, std::string> consume(T value) = 0;
};

template <typename T>
struct ICliArg : IOption {
    virtual ~ICliArg() = default;
    virtual Result<Unit, std::string> apply(IConsumer<T>* consumer) = 0;
};

// Specialization for arbitrarily complex types
template <typename Description, typename T>
struct CliArg : ICliArg<T>, IConsumer<typename ComplexType<T>::type> {
    using Traits = ComplexType<T>;
    using U = typename Traits::type;

    CliArg() = default;

    // IOption
    void attach(TCLAP::CmdLine& cmd) override {
        impl_.attach(cmd);
    }

    // IConsumer<U>
    Result<Unit, std::string> consume(U value) override {
        auto lifted = Traits::lift(std::move(value));

        if (!lifted) {
            return std::unexpected(lifted.error());
        }

        return consumer_->consume(*lifted);
    }

    // ICliArg<T>
    Result<Unit, std::string> apply(IConsumer<T>* consumer) override {
        consumer_ = consumer;
        return impl_.apply(this);
    }

 private:
    IConsumer<T>* consumer_ = nullptr;
    CliArg<Description, U> impl_;
};

// Regular types (int, string, float, ...)
template <typename Description, BasicType T>
struct CliArg<Description, T> : ICliArg<T> {
    CliArg()
        : arg_(
              std::string(1, Description::Short),
              Description::Long.c_str(),
              Description::Description.c_str(),
              false,
              T{},
              rfl::type_name_t<T>().name()) {}

    // IOption
    void attach(TCLAP::CmdLine& cmd) override {
        cmd.add(arg_);
    }

    // ICliArg<T>
    Result<Unit, std::string> apply(IConsumer<T>* consumer) override {
        if (!arg_.isSet()) {
            return unit;
        }

        return consumer->consume(arg_.getValue());
    }

 private:
    TCLAP::ValueArg<T> arg_;
};

// Boolean types are just switches
template <typename Description>
struct CliArg<Description, bool> : ICliArg<bool> {
    CliArg()
        : arg_(
              std::string(1, Description::Short),
              Description::Long.c_str(),
              Description::Description.c_str()) {}

    // IOption
    void attach(TCLAP::CmdLine& cmd) override {
        cmd.add(arg_);
    }

    // ICliArg<bool>
    Result<Unit, std::string> apply(IConsumer<bool>* consumer) override {
        if (!arg_.isSet()) {
            return unit;
        }

        return consumer->consume(arg_.getValue());
    }

 private:
    TCLAP::SwitchArg arg_;
};

// Specialization for vectors of basic types
template <typename Description, BasicType T>
struct CliArg<Description, std::vector<T>> : ICliArg<std::vector<T>> {
    CliArg()
        : arg_(
              std::string(1, Description::Short),
              Description::Long.c_str(),
              Description::Description.c_str(),
              false,
              rfl::type_name_t<T>().name()) {}

    // IOption
    void attach(TCLAP::CmdLine& cmd) override {
        cmd.add(arg_);
    }

    // ICliArg
    Result<Unit, std::string> apply(IConsumer<std::vector<T>>* consumer) override {
        return consumer->consume(arg_.getValue());
    }

 private:
    TCLAP::MultiArg<T> arg_;
};

}  // namespace pdft::rt::cli::detail
