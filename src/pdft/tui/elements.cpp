#include "pdft/tui/elements.h"

namespace pdft::tui {

namespace {

class ReflectRect : public Node {
 public:
    ReflectRect(Element child, IRect& rect)
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
        reflected_rect_ = toRect(Box::Intersection(screen.stencil, box_));
        return Node::Render(screen);
    }

 private:
    IRect& reflected_rect_;
};

}  // namespace

IRect toRect(const Box& box) {
    return {
        .x = static_cast<uint32_t>(box.x_min),
        .y = static_cast<uint32_t>(box.y_min),
        .width = static_cast<uint32_t>(box.x_max - box.x_min + 1),
        .height = static_cast<uint32_t>(box.y_max - box.y_min + 1),
    };
}

Decorator reflect(IRect& rect) {
    return [&](ftxui::Element child) -> ftxui::Element {
        return std::make_shared<ReflectRect>(std::move(child), rect);
    };
}

Element empty(IRect& rect) {
    return ftxui::emptyElement() | reflect(rect);
}

}  // namespace pdft::tui
