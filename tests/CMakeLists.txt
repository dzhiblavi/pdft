include(Catch)

find_package(Catch2 REQUIRED)
find_package(GTest REQUIRED)

function(add_test path)
  string(REPLACE ".cpp" "" name ${path})
  string(REPLACE "./" "" name ${name})
  string(REPLACE "/" "_" name ${name})

  add_executable("test_${name}" "${path}" ${ARGN})

  target_link_libraries(test_${name} PRIVATE pdft-lib Catch2::Catch2WithMain
                                             GTest::gmock)
  target_include_directories(test_${name}
                             PRIVATE $<BUILD_INTERFACE:${PDFT_SOURCE_DIR}/>)

  catch_discover_tests(test_${name} DISCOVERY_MODE PRE_TEST WORKING_DIRECTORY
                       ${CMAKE_CURRENT_SOURCE_DIR})
endfunction(add_test)

# Add tests here

# core
add_test(./pdft/core/trace/test_scope_tracer.cpp)
add_test(./pdft/core/trace/test_stack_scope_tracer.cpp)
add_test(./pdft/core/cli/detail/test_cli_args.cpp)

# core/result
add_test(./pdft/core/result/combine/test_and_then.cpp)
add_test(./pdft/core/result/combine/test_lift.cpp)
add_test(./pdft/core/result/combine/test_map.cpp)
add_test(./pdft/core/result/combine/test_map_err.cpp)
add_test(./pdft/core/result/combine/test_or_else.cpp)
add_test(./pdft/core/result/coro/test_maybe.cpp)
add_test(./pdft/core/result/coro/test_result.cpp)

# io
add_test(./pdft/io/test_posix_console_writer.cpp)
add_test(./pdft/io/test_posix_console_reader.cpp)

# image
add_test(./pdft/image/test_color.cpp)

# utils
add_test(./pdft/utils/test_lock_free_stack.cpp)
add_test(./pdft/utils/test_lru_cache.cpp)

# tui
add_test(./pdft/tui/test_alignment.cpp)
add_test(./pdft/tui/test_fit_image.cpp)
add_test(./pdft/tui/test_ansi_terminal.cpp)
add_test(./pdft/tui/kitty/apc/test_parse_response.cpp)
add_test(./pdft/tui/kitty/apc/test_apc_executor_base.cpp)
add_test(./pdft/tui/kitty/apc/test_tmux_apc_executor.cpp)
add_test(./pdft/tui/kitty/apc/test_regular_apc_executor.cpp)

# app
add_test(./pdft/app/controller/test_keytree.cpp)
