# Common compile options for C++

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so")

# https://clang.llvm.org/docs/DiagnosticsReference.html
add_compile_options(-Wall -Wextra -Wpedantic -fno-omit-frame-pointer)

# Turn warnings into errors
add_compile_options(-Wno-language-extension-token)

add_compile_options(-Wno-error=unused-command-line-argument)

# Add debug symbols event in Release build
add_compile_options(-g -gdwarf-4)

# libc++
add_compile_options(-stdlib=libc++)
add_link_options(-stdlib=libc++)

message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
