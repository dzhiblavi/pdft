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

add_test(./pdft/core/io/posix/test_console.cpp)
add_test(./pdft/core/terminal/ansi/test_terminal.cpp)
add_test(./pdft/core/terminal/tmux/test_apc_executor.cpp)
add_test(./pdft/core/terminal/regular/test_apc_executor.cpp)
add_test(./pdft/core/trace/test_scope_tracer.cpp)
add_test(./pdft/core/trace/test_stack_scope_tracer.cpp)

add_test(./pdft/utils/test_intrusive_forward_list.cpp)

add_test(./pdft/view/test_layout.cpp)

add_test(./pdft/display/test_fit.cpp)

add_test(./pdft/controller/test_keytree.cpp)
