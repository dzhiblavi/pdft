add_library(pdft-event OBJECT)

target_link_libraries(pdft-event PUBLIC pdft-core)

target_sources(pdft-event PRIVATE ./key_event_listener.cpp
                                  ./signal_event_listener.cpp)
