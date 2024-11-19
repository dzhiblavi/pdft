find_package(yaml-cpp REQUIRED)

include(FetchContent)

set(REFLECTCPP_BSON OFF)
set(REFLECTCPP_CBOR OFF)
set(REFLECTCPP_FLEXBUFFERS OFF)
set(REFLECTCPP_MSGPACK OFF)
set(REFLECTCPP_TOML OFF)
set(REFLECTCPP_UBJSON OFF)
set(REFLECTCPP_XML OFF)
set(REFLECTCPP_YAML ON)

FetchContent_Declare(
  reflect_cpp
  GIT_REPOSITORY https://github.com/getml/reflect-cpp.git
  GIT_TAG v0.15.0
  GIT_PROGRESS TRUE
  INSTALL_COMMAND "")

FetchContent_GetProperties(reflect_cpp)
FetchContent_MakeAvailable(reflect_cpp)

# Link reflectcpp to yaml-cpp to it finds the library
target_link_libraries(reflectcpp PUBLIC yaml-cpp::yaml-cpp)
