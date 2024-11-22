find_package(tclap REQUIRED)

add_executable(
  app
  ./main.cpp
  ./app.cpp
  ./logging.cpp
  ./setup_signal_handlers.cpp
  ./cli_args.cpp
  ./resolve_apc_executor.cpp
  ./resolve_display.cpp)

target_link_libraries(app pdft tclap::tclap reflectcpp)
