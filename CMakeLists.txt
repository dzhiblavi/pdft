cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(
  pdft
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PDFT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Third-party libraries
include(./cmake/thirdparty/MuPDF.cmake)
include(./cmake/thirdparty/fswatch.cmake)

# Project settings
include(./cmake/StandardProjectSettings.cmake)
include(./cmake/EnableCompilerWarnings.cmake)

add_subdirectory(src)

# Enable testing if needed
include(./cmake/Testing.cmake)
