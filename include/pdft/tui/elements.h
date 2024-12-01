#pragma once

#include "pdft/tui/rect.h"

#include <ftxui/dom/elements.hpp>

namespace pdft::tui {

inline IRect toRect(const ftxui::Box& box) {
    return {
        .x = static_cast<unsigned int>(box.x_min),
        .y = static_cast<unsigned int>(box.y_min),
        .width = static_cast<unsigned int>(box.x_max - box.x_min + 1),
        .height = static_cast<unsigned int>(box.y_max - box.y_min + 1),
    };
}

template <typename F>
struct WithSize : ftxui::Node {
 public:
    explicit WithSize(F f) : func_(std::move(f)) {}

    void Render(ftxui::Screen& screen) {
        std::move(func_)(toRect(ftxui::Box::Intersection(screen.stencil, box_)));
    }

 private:
    tui::IRect rect_;
    [[no_unique_address]] F func_;
};

class ReflectRect : public ftxui::Node {
 public:
    ReflectRect(ftxui::Element child, IRect& rect)
        : Node(unpack(std::move(child)))
        , reflected_rect_(rect) {}

    void ComputeRequirement() final {
        Node::ComputeRequirement();
        requirement_ = children_[0]->requirement();
    }

    void SetBox(ftxui::Box box) final {
        reflected_rect_ = toRect(box);
        Node::SetBox(box);
        children_[0]->SetBox(box);
    }

    void Render(ftxui::Screen& screen) final {
        reflected_rect_ = toRect(ftxui::Box::Intersection(screen.stencil, box_));
        return Node::Render(screen);
    }

 private:
    IRect& reflected_rect_;
};

using ftxui::reflect;

inline ftxui::Decorator reflect(IRect& rect) {
    return [&](ftxui::Element child) -> ftxui::Element {
        return std::make_shared<ReflectRect>(std::move(child), rect);
    };
}

template <std::invocable<IRect> F>
ftxui::Element withSize(F f) {
    return std::make_shared<WithSize<F>>(std::move(f));
}

using Element = ftxui::Element;
using Box = ftxui::Box;

using ftxui::size;

using ftxui::border;
using ftxui::flex;

using ftxui::emptyElement;
using ftxui::hbox;
using ftxui::text;
using ftxui::vbox;
using ftxui::window;

using ftxui::Render;

using enum ftxui::WidthOrHeight;
using enum ftxui::Constraint;

}  // namespace pdft::tui
