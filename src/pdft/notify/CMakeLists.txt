add_library(pdft-notify OBJECT ./fsw_notify.cpp)

target_link_libraries(pdft-notify PUBLIC pdft-core libfswatch)
