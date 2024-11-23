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
         cppcoro::cppcoro)

target_sources(
  pdft
  PRIVATE ./utils/base64.cpp
          ./utils/zlib.cpp
          ./utils/get_home_dir.cpp
          ./view/rect.cpp
          ./view/layout.cpp
          ./view/axis_fit.cpp
          ./view/app/fit_page.cpp
          ./view/app/fit_text.cpp
          ./view/app/renderer.cpp
          ./state/app/state.cpp
          ./state/app/document_state.cpp
          ./state/document_storage.cpp
          ./controller/keytree.cpp
          ./controller/app/controller.cpp
          ./core/assert.cpp
          ./core/log/load.cpp
          ./core/log/log.cpp
          ./core/trace/stack_scope_tracer.cpp
          ./core/io/posix/console.cpp
          ./core/io/posix/terminal.cpp
          ./core/terminal/terminal.cpp
          ./core/terminal/ansi/terminal.cpp
          ./core/apc/parse_response.cpp
          ./core/apc/apc_executor_base.cpp
          ./core/apc/regular/apc_executor.cpp
          ./core/apc/tmux/apc_executor.cpp
          ./display/kitty/display_base.cpp
          ./display/kitty/unicode_display.cpp
          ./display/kitty/native_display.cpp
          ./media/pdf/context.cpp
          ./media/pdf/document.cpp
          ./media/pdf/page.cpp
          ./media/pdf/pixmap_image.cpp)
