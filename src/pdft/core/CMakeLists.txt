add_library(pdft-core STATIC)
target_link_libraries(pdft-core PUBLIC platform pdft-utils)
target_sources(pdft-core PRIVATE ./context.cpp)
