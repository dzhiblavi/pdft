find_package(asio REQUIRED)
find_package(base64 REQUIRED)
find_package(spdlog REQUIRED)
find_package(cpptrace REQUIRED)
find_package(ZLIB REQUIRED)
find_package(magic_enum REQUIRED)
find_package(cppcoro REQUIRED)

add_library(pdft-buildinfo)
target_include_directories(
  pdft-buildinfo PUBLIC $<BUILD_INTERFACE:${PDFT_SOURCE_DIR}/include/>)
target_sources(pdft-buildinfo PRIVATE ${BUILD_INFO_SOURCE_PATH})

add_library(pdft)

target_include_directories(
  pdft PUBLIC $<BUILD_INTERFACE:${PDFT_SOURCE_DIR}/include/>)
target_include_directories(
  pdft SYSTEM PUBLIC $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>)

target_link_libraries(
  pdft
  PUBLIC pdft-buildinfo
         asio::asio
         aklomp::base64
         spdlog::spdlog
         cpptrace::cpptrace
         ZLIB::ZLIB
         libmupdf_ext
         libfswatch
         magic_enum::magic_enum
         cppcoro::cppcoro
         reflectcpp
         ftxui::screen
         ftxui::dom)

target_sources(
  pdft PRIVATE ./utils/base64.cpp ./utils/zlib.cpp ./utils/get_home_dir.cpp
               ./utils/random.cpp ./utils/base64.cpp)

target_sources(
  pdft
  PRIVATE ./core/assert.cpp
          ./core/context.cpp
          ./core/log/load.cpp
          ./core/log/log.cpp
          ./core/trace/stack_scope_tracer.cpp
          ./core/trace/log_stack_trace.cpp
          ./core/io/posix/console.cpp
          ./core/io/posix/terminal.cpp)

target_sources(pdft PRIVATE ./notify/fsw_notify.cpp)

target_sources(pdft PRIVATE ./image/operations.cpp ./image/palette.cpp)

target_sources(
  pdft
  PRIVATE ./media/mupdf/context.cpp ./media/mupdf/document.cpp
          ./media/mupdf/page.cpp ./media/mupdf/pixmap_image.cpp
          ./media/mupdf/loader.cpp)

target_sources(
  pdft
  PRIVATE ./tui/fit_image.cpp
          ./tui/terminal.cpp
          ./tui/ansi_terminal.cpp
          ./tui/kitty/apc/parse_response.cpp
          ./tui/kitty/apc/apc_executor_base.cpp
          ./tui/kitty/apc/regular_apc_executor.cpp
          ./tui/kitty/apc/tmux_apc_executor.cpp
          ./tui/kitty/display_base.cpp
          ./tui/kitty/unicode_display.cpp
          ./tui/kitty/native_display.cpp)

target_sources(
  pdft
  PRIVATE ./app/state/ui_state.cpp
          ./app/state/document_storage.cpp
          ./app/view/impl/page_renderer.cpp
          ./app/view/impl/view.cpp
          ./app/view/impl/document_view.cpp
          ./app/controller/impl/keytree.cpp
          ./app/controller/impl/controller.cpp
          ./app/loader/impl/loader.cpp
          ./app/persist/impl/persister.cpp)
