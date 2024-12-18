find_package(asio REQUIRED)

# PDFT executable
add_executable(pdft ./main.cpp ./version.cpp)

# Force link to preseve global module declarations
target_force_link_libraries(pdft PRIVATE pdft-lib asio::asio)
