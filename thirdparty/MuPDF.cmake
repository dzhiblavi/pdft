include(ExternalProject)
include(FetchContent)

# Download mupdf source code
FetchContent_Declare(
  mupdf_ext
  GIT_REPOSITORY "https://github.com/ArtifexSoftware/mupdf.git"
  GIT_TAG 1.24.10
  GIT_SUBMODULES_RECURSE 1)

FetchContent_GetProperties(mupdf_ext)
FetchContent_MakeAvailable(mupdf_ext)
set(MUPDF_SRC_DIR ${CMAKE_BINARY_DIR}/_deps/mupdf_ext-src)

# Build mupdf using make
ExternalProject_Add(
  mupdf_ext
  SOURCE_DIR "${mupdf_ext_SOURCE_DIR}"
  PREFIX ext
  BUILD_IN_SOURCE 1
  BUILD_COMMAND make HAVE_GLUT=0 HAVE_X11=0 -j
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND "")

# Add custom mupdf target
add_library(mupdf STATIC IMPORTED GLOBAL)
set_target_properties(
  mupdf PROPERTIES IMPORTED_LOCATION
                   "${MUPDF_SRC_DIR}/build/release/libmupdf.a")

set(MUPDF_ROOT ${CMAKE_BINARY_DIR}/mupdf_external-prefix)
set(MUPDF_LIB_DIR "${MUPDF_SRC_DIR}/build/release")
target_link_libraries(
  mupdf
  INTERFACE "${MUPDF_LIB_DIR}/libmupdf-pkcs7.a"
            "${MUPDF_LIB_DIR}/libmupdf-third.a"
            "${MUPDF_LIB_DIR}/libmupdf-threads.a")

target_include_directories(mupdf INTERFACE "${MUPDF_SRC_DIR}/include")
