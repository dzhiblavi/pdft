include(FetchContent)

FetchContent_Declare(
  fswatch_ext
  GIT_REPOSITORY https://github.com/emcrisostomo/fswatch.git
  GIT_TAG 1.17.1
  GIT_PROGRESS TRUE
  INSTALL_COMMAND "")

FetchContent_GetProperties(fswatch_ext)
FetchContent_MakeAvailable(fswatch_ext)

# Build fswatch using make ExternalProject_Add( fswatch_ext SOURCE_DIR
# "${fswatch_ext_SOURCE_DIR}" PREFIX ext BUILD_IN_SOURCE 1 CONFIGURE_COMMAND
# ./autogen.sh && ./configure BUILD_COMMAND make -j INSTALL_COMMAND "")

# Add custom fswatch target add_library(libfswatch_ext STATIC IMPORTED GLOBAL)

# target_include_directories(libfswatch_ext INTERFACE
# "${fswatch_ext_SOURCE_DIR}/libfswatch/src")

# target_include_directories(libfswatch_ext INTERFACE
# "${fswatch_ext_SOURCE_DIR}/libfswatch")

# set_target_properties( libfswatch_ext PROPERTIES IMPORTED_LOCATION
# "${fswatch_ext_SOURCE_DIR}/libfswatch/src/.libs/libfswatch.a")
