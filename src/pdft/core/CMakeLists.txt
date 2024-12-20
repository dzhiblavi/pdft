find_package(asio REQUIRED)
find_package(spdlog REQUIRED)
find_package(cpptrace REQUIRED)
find_package(magic_enum REQUIRED)
find_package(tclap REQUIRED)

add_library(pdft-core STATIC)

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
  pdft-core PRIVATE ./assert.cpp ./context.cpp ./log/load.cpp ./log/log.cpp
                    ./trace/stack_scope_tracer.cpp ./trace/log_stack_trace.cpp)
