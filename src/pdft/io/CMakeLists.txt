add_library(pdft-io OBJECT)

target_link_libraries(pdft-io PUBLIC pdft-core)

target_sources(pdft-io PRIVATE ./posix_console_writer.cpp
                               ./posix_console_reader.cpp ./posix_terminal.cpp)
