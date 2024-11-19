add_executable(app ./app.cpp ./setup_logging.cpp ./setup_signal_handlers.cpp
                   ./consume_fs_events.cpp ./keyboard_consumer.cpp ./main.cpp)

target_link_libraries(app pdft)
