#pragma once

#include "pdft/utils/cli_args/detail/lift.h"

#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/unit.h"

#include <tclap/CmdLine.h>

namespace pdft::cli::detail {

template <typename T>
struct IConsumer {
    virtual ~IConsumer() = default;
    virtual Result<Unit, std::string> consume(T value) = 0;
};

template <typename T>
struct ICliArg {
    virtual ~ICliArg() = default;
    virtual Result<Unit, std::string> apply(IConsumer<T>* consumer) = 0;
};

// Specialization for arbitrarily complex types
template <typename Description, typename T>
struct CliArg : ICliArg<T>, IConsumer<typename ComplexType<T>::type> {
    using Traits = ComplexType<T>;
    using U = typename Traits::type;

    CliArg(TCLAP::CmdLine& line) : impl_(line) {}

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
    CliArg(TCLAP::CmdLine& line)
        : arg_(
              std::string(1, Description::Short),
              Description::Long.c_str(),
              Description::Description.c_str(),
              false,
              T{},
              "",
              line) {}

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
    CliArg(TCLAP::CmdLine& line)
        : arg_(
              std::string(1, Description::Short),
              Description::Long.c_str(),
              Description::Description.c_str(),
              line) {}

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
    CliArg(TCLAP::CmdLine& line)
        : arg_(
              std::string(1, Description::Short),
              Description::Long.c_str(),
              Description::Description.c_str(),
              false,
              "",
              line) {}

    // ICliArg
    Result<Unit, std::string> apply(IConsumer<std::vector<T>>* consumer) override {
        return consumer->consume(arg_.getValue());
    }

 private:
    TCLAP::MultiArg<T> arg_;
};

}  // namespace pdft::cli::detail
