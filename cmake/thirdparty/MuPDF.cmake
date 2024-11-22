include(ExternalProject)
include(FetchContent)

# Download mupdf source code
FetchContent_Declare(
  mupdf_ext
  GIT_REPOSITORY "https://github.com/ArtifexSoftware/mupdf.git"
  GIT_TAG 1.24.10
  GIT_SUBMODULES_RECURSE 1
  GIT_PROGRESS TRUE)

FetchContent_GetProperties(mupdf_ext)
FetchContent_MakeAvailable(mupdf_ext)

# Release -> release, Debug -> debug
string(TOLOWER "${CMAKE_BUILD_TYPE}" MUPDF_BUILD_TYPE)

# Build mupdf using make
ExternalProject_Add(
  mupdf_ext
  SOURCE_DIR "${mupdf_ext_SOURCE_DIR}"
  PREFIX ext
  BUILD_IN_SOURCE 1
  BUILD_COMMAND make HAVE_GLUT=0 HAVE_X11=0 build=${MUPDF_BUILD_TYPE} -j
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND "")

# Add custom mupdf target
add_library(libmupdf_ext STATIC IMPORTED GLOBAL)

set(MUPDF_SRC_DIR ${CMAKE_BINARY_DIR}/_deps/mupdf_ext-src)
set(MUPDF_LIB_DIR "${MUPDF_SRC_DIR}/build/${MUPDF_BUILD_TYPE}")

set_target_properties(
  libmupdf_ext
  PROPERTIES IMPORTED_LOCATION
             "${MUPDF_SRC_DIR}/build/${MUPDF_BUILD_TYPE}/libmupdf.a")

target_link_libraries(
  libmupdf_ext
  INTERFACE "${MUPDF_LIB_DIR}/libmupdf-pkcs7.a"
            "${MUPDF_LIB_DIR}/libmupdf-third.a"
            "${MUPDF_LIB_DIR}/libmupdf-threads.a")

target_include_directories(libmupdf_ext INTERFACE "${MUPDF_SRC_DIR}/include")

# Set library's include directory as SYSTEM so it does not raise unneeded
# warnings
set_target_properties(
  libmupdf_ext
  PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
             $<TARGET_PROPERTY:libmupdf_ext,INTERFACE_INCLUDE_DIRECTORIES>)
