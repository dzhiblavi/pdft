add_library(project_warnings INTERFACE)

# I always treat warnings as errors, making my life easier in the long run.
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" TRUE)
message(STATUS "Treat compiler warnings as errors")

set(MSVC_WARNINGS /W4)
set(CLANG_WARNINGS -Wall -Wextra)

# Stop the build if there are any warnings,
if(WARNINGS_AS_ERRORS)
  set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
  set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
endif()

set(GCC_WARNINGS ${CLANG_WARNINGS})

if(MSVC)
  set(PROJECT_WARNINGS ${MSVC_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(PROJECT_WARNINGS ${CLANG_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(PROJECT_WARNINGS ${GCC_WARNINGS})
else()
  message(
    AUTHOR_WARNING
      "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

target_compile_options(project_warnings INTERFACE ${PROJECT_WARNINGS})
