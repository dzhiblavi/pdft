find_package(tclap REQUIRED)

add_executable(
  app
  ./main.cpp
  ./app.cpp
  ./logging.cpp
  ./cli_args.cpp
  ./resolve_display.cpp
  ./version.cpp)

target_link_libraries(app pdft tclap::tclap reflectcpp)
