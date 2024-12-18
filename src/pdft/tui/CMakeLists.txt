add_library(
  pdft-tui OBJECT
  ./fit_image.cpp
  ./terminal.cpp
  ./ansi_terminal.cpp
  ./app_renderer.cpp
  ./elements.cpp
  ./kitty/apc/parse_response.cpp
  ./kitty/apc/apc_executor_base.cpp
  ./kitty/apc/regular_apc_executor.cpp
  ./kitty/apc/tmux_apc_executor.cpp
  ./kitty/apc/apc_executor_factory.cpp
  ./kitty/unicode_kitty_display.cpp
  ./kitty/native_kitty_display.cpp
  ./kitty/kitty_display_base.cpp
  ./kitty/kitty_display_factory.cpp)

target_link_libraries(pdft-tui PUBLIC pdft-core pdft-io ftxui::screen
                                      ftxui::dom)
