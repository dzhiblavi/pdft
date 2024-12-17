add_library(pdft-image STATIC)

target_link_libraries(pdft-image PUBLIC pdft-core)
target_sources(pdft-image PRIVATE ./operations.cpp ./palette.cpp)
