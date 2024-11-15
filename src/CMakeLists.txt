add_subdirectory(pdft)

add_executable(app ./app.cpp)
target_link_libraries(app pdft)
