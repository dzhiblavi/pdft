find_package(asio REQUIRED)
find_package(spdlog REQUIRED)
find_package(cpptrace REQUIRED)
find_package(magic_enum REQUIRED)
find_package(tclap REQUIRED)

add_library(pdft-core OBJECT)

target_include_directories(
  pdft-core PUBLIC $<BUILD_INTERFACE:${PDFT_SOURCE_DIR}/include/>)
target_include_directories(
  pdft-core SYSTEM PUBLIC $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>)

target_link_libraries(
  pdft-core
  PUBLIC pdft-utils
         dl # cpptrace dependency
         saf
         reflectcpp
         asio::asio
         tclap::tclap
         spdlog::spdlog
         cpptrace::cpptrace
         magic_enum::magic_enum)

target_sources(
  pdft-core
  PRIVATE ./assert.cpp
          ./module/load.cpp
          ./module/factory_storage.cpp
          ./module/start.cpp
          ./context.cpp
          ./log/load.cpp
          ./log/log.cpp
          ./trace/stack_scope_tracer.cpp
          ./trace/log_stack_trace.cpp
          ./io/posix_console_writer.cpp
          ./io/posix_console_reader.cpp
          ./io/posix_terminal.cpp
          ./cli/parse.cpp
          ./cli/detail/storage.cpp)
