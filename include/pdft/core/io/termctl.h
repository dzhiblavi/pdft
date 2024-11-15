#pragma once

#include <optional>

namespace pdft::core::io {

enum class ControlType {
    Input,
    Output,
};

struct IoControl {
    std::optional<bool> canonical;
    std::optional<bool> echo;
};

class TermControl {
 public:
    virtual ~TermControl() = default;

    virtual void ioControl(ControlType type, const IoControl& control) = 0;
};

}  // namespace pdft::core::io
