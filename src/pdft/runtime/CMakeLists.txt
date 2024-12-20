add_library(pdft-runtime OBJECT)

target_link_libraries(pdft-runtime PUBLIC pdft-core)

target_sources(
  pdft-runtime
  PRIVATE ./module/load.cpp
          ./module/factory_storage.cpp
          ./module/start.cpp
          ./cli/parse.cpp
          ./cli/storage.cpp
          ./event/key_event_emitter.cpp
          ./event/signal_event_emitter.cpp)
