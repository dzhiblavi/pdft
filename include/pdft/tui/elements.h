#pragma once

#include "pdft/tui/rect.h"

#include <ftxui/dom/elements.hpp>

namespace pdft::tui {

using namespace ftxui;

inline IRect toRect(const Box& box) {
    return {
        .x = static_cast<unsigned int>(box.x_min),
        .y = static_cast<unsigned int>(box.y_min),
        .width = static_cast<unsigned int>(box.x_max - box.x_min + 1),
        .height = static_cast<unsigned int>(box.y_max - box.y_min + 1),
    };
}

template <typename F>
struct WithSize : Node {
 public:
    explicit WithSize(F f) : func_(std::move(f)) {}

    void Render(Screen& screen) {
        std::move(func_)(toRect(Box::Intersection(screen.stencil, box_)));
    }

 private:
    tui::IRect rect_;
    [[no_unique_address]] F func_;
};

class ReflectRect : public Node {
 public:
    ReflectRect(Element child, IRect& rect)
        : Node(unpack(std::move(child)))
        , reflected_rect_(rect) {}

    void ComputeRequirement() final {
        Node::ComputeRequirement();
        requirement_ = children_[0]->requirement();
    }

    void SetBox(Box box) final {
        reflected_rect_ = toRect(box);
        Node::SetBox(box);
        children_[0]->SetBox(box);
    }

    void Render(Screen& screen) final {
        reflected_rect_ = toRect(Box::Intersection(screen.stencil, box_));
        return Node::Render(screen);
    }

 private:
    IRect& reflected_rect_;
};

using ftxui::reflect;

inline Decorator reflect(IRect& rect) {
    return [&](Element child) -> Element {
        return std::make_shared<ReflectRect>(std::move(child), rect);
    };
}

template <std::invocable<IRect> F>
ftxui::Element withSize(F f) {
    return std::make_shared<WithSize<F>>(std::move(f));
}

}  // namespace pdft::tui
