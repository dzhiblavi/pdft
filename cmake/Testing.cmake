option(PDFT_BUILD_TESTS "Build tests" ON)

if(PDFT_BUILD_TESTS AND (PDFT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
  enable_testing()
  add_subdirectory(tests)
endif()
