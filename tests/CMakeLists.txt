include(Catch)
find_package(Catch2 REQUIRED)

function(add_test name)
  add_executable(${name} "test_${name}.cpp" ${ARGN})
  target_link_libraries(${name} PRIVATE pdft Catch2::Catch2WithMain)
  catch_discover_tests(${name} DISCOVERY_MODE PRE_TEST WORKING_DIRECTORY
                       ${CMAKE_CURRENT_SOURCE_DIR})
endfunction(add_test)

# Add subdirectories with tests here

add_subdirectory(pdft)
