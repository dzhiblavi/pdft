find_package(asio REQUIRED)
find_package(base64 REQUIRED)
find_package(spdlog REQUIRED)
find_package(cpptrace REQUIRED)
find_package(ZLIB REQUIRED)
find_package(magic_enum REQUIRED)

add_library(pdft)

target_include_directories(
  pdft PUBLIC $<BUILD_INTERFACE:${PDFT_SOURCE_DIR}/include/>)
target_include_directories(
  pdft SYSTEM PUBLIC $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>)

target_link_libraries(
  pdft
  PUBLIC asio::asio
         aklomp::base64
         spdlog::spdlog
         cpptrace::cpptrace
         ZLIB::ZLIB
         libmupdf_ext
         libfswatch
         magic_enum::magic_enum)

target_sources(
  pdft
  PRIVATE ./utils/base64.cpp
          ./utils/zlib.cpp
          ./view/rect.cpp
          ./view/layout.cpp
          ./view/app/renderer.cpp
          ./state/app/state.cpp
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
          ./core/terminal/regular/apc_executor.cpp
          ./core/terminal/tmux/apc_executor.cpp
          ./media/pdf/context.cpp
          ./media/pdf/document.cpp
          ./media/pdf/page.cpp
          ./media/pdf/pixmap_image.cpp
          ./display/fit.cpp
          ./display/kitty/display.cpp)
