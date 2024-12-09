#pragma once

#include "pdft/tui/rect.h"

#include <ftxui/dom/elements.hpp>

namespace pdft::tui {

using Element = ftxui::Element;
using Decorator = ftxui::Decorator;
using Box = ftxui::Box;
using Node = ftxui::Node;

using enum ftxui::WidthOrHeight;
using enum ftxui::Constraint;

using ftxui::border;
using ftxui::flex;
using ftxui::reflect;
using ftxui::size;

using ftxui::emptyElement;
using ftxui::hbox;
using ftxui::text;
using ftxui::vbox;
using ftxui::window;

using ftxui::Render;

IRect toRect(const Box& box);

Decorator reflect(IRect& rect);

Element empty(IRect& rect);

}  // namespace pdft::tui
