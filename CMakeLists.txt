cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(
  framework
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(FRAMEWORK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(./cmake/StandardProjectSettings.cmake)
include(./cmake/EnableCompilerWarnings.cmake)

add_subdirectory(src)

include(./cmake/Testing.cmake)
