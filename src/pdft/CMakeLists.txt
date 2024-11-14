add_library(pdft)

target_include_directories(
  pdft PUBLIC $<BUILD_INTERFACE:${FRAMEWORK_SOURCE_DIR}/include/>)
target_include_directories(
  pdft SYSTEM PUBLIC $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>)

target_sources(pdft PRIVATE ./view/layout.cpp)
