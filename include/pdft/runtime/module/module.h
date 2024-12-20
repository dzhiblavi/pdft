#pragma once

#include "pdft/core/log/with_logger.h"

#include <string>

namespace pdft::rt {

class Module : public log::WithLogger {
 public:
    Module() = default;

    virtual ~Module() = default;

    virtual void start() {}

    void setName(std::string name) {
        module_name = std::move(name);
    }

    const std::string& name() {
        return module_name;
    }

 private:
    std::string module_name;
};

template <typename T>
concept IsModule = std::derived_from<T, Module>;

}  // namespace pdft::rt
