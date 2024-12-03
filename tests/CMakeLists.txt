include(Catch)
find_package(Catch2 REQUIRED)
find_package(GTest REQUIRED)

function(add_test path)
  string(REPLACE ".cpp" "" name ${path})
  string(REPLACE "./" "" name ${name})
  string(REPLACE "/" "_" name ${name})

  add_executable("test_${name}" "${path}" ${ARGN})

  target_link_libraries(test_${name} PRIVATE pdft Catch2::Catch2WithMain
                                             GTest::gmock)
  target_include_directories(test_${name}
                             PRIVATE $<BUILD_INTERFACE:${PDFT_SOURCE_DIR}/>)

  catch_discover_tests(test_${name} DISCOVERY_MODE PRE_TEST WORKING_DIRECTORY
                       ${CMAKE_CURRENT_SOURCE_DIR})
endfunction(add_test)

# Add tests here

# Core
add_test(./pdft/core/io/posix/test_console.cpp)
add_test(./pdft/core/trace/test_scope_tracer.cpp)
add_test(./pdft/core/trace/test_stack_scope_tracer.cpp)

add_test(./pdft/core/result/coro/test_maybe.cpp)
add_test(./pdft/core/result/coro/test_result.cpp)

# Image
add_test(./pdft/image/color.cpp)

# Utils
add_test(./pdft/utils/test_intrusive_forward_list.cpp)

# TUI
add_test(./pdft/tui/test_alignment.cpp)
add_test(./pdft/tui/image/test_fit_image.cpp)
add_test(./pdft/tui/terminal/ansi/test_terminal.cpp)
add_test(./pdft/tui/display/kitty/apc/test_parse_response.cpp)
add_test(./pdft/tui/display/kitty/apc/test_executor_base.cpp)
add_test(./pdft/tui/display/kitty/apc/tmux/test_executor.cpp)
add_test(./pdft/tui/display/kitty/apc/regular/test_executor.cpp)

# Application
add_test(./pdft/app/controller/test_keytree.cpp)
