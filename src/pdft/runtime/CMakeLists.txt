add_library(pdft-runtime OBJECT)

target_link_libraries(pdft-runtime PUBLIC pdft-core)

target_sources(
  pdft-runtime PRIVATE ./event/key_event_emitter.cpp
                       ./event/signal_event_emitter.cpp ./start.cpp)
