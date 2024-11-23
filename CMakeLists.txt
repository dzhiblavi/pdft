cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(
  pdft
  VERSION 0.0.1
  LANGUAGES CXX)

set(PDFT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Global settings
include(./cmake/Ccache.cmake)
include(./cmake/CheckCompiler.cmake)
include(./cmake/Development.cmake)
include(./cmake/Sanitizers.cmake)

# Third-party libraries
include(./cmake/thirdparty/MuPDF.cmake)
include(./cmake/thirdparty/fswatch.cmake)
include(./cmake/thirdparty/reflect-cpp.cmake)

# Enable warnings for core of the project
include(./cmake/CompileOptions.cmake)
include(./cmake/Werror.cmake)

include(./cmake/GenerateBuildInfo.cmake)

add_subdirectory(src)

include(./cmake/Testing.cmake)
