find_package(base64 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Boost REQUIRED)

# Build info as a separate library
add_library(pdft-utils-buildinfo STATIC)
target_include_directories(
  pdft-utils-buildinfo PUBLIC $<BUILD_INTERFACE:${PDFT_SOURCE_DIR}/include/>)
target_include_directories(
  pdft-utils-buildinfo SYSTEM
  PUBLIC $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>)
target_sources(pdft-utils-buildinfo PRIVATE ${BUILD_INFO_SOURCE_PATH})

add_library(pdft-utils STATIC)

target_link_libraries(pdft-utils PUBLIC pdft-utils-buildinfo Boost::boost)
target_link_libraries(pdft-utils PRIVATE aklomp::base64 ZLIB::ZLIB)

target_sources(pdft-utils PRIVATE ./base64.cpp ./zlib.cpp ./get_home_dir.cpp
                                  ./random.cpp ./base64.cpp)
