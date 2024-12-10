find_package(tclap REQUIRED)

add_executable(app ./main.cpp ./app.cpp ./cli_args.cpp ./version.cpp
                   ./palettes.cpp ./entrypoint.cpp)

target_link_libraries(app pdft tclap::tclap reflectcpp)
