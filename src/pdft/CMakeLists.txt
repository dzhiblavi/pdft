add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(io)
add_subdirectory(notify)
add_subdirectory(image)
add_subdirectory(media)
add_subdirectory(tui)
add_subdirectory(app)

add_library(
  pdft-lib STATIC
  $<TARGET_OBJECTS:pdft-io> $<TARGET_OBJECTS:pdft-notify>
  $<TARGET_OBJECTS:pdft-media> $<TARGET_OBJECTS:pdft-tui>
  $<TARGET_OBJECTS:pdft-app>)

target_link_libraries(
  pdft-lib
  INTERFACE pdft-core
            pdft-io
            pdft-utils
            pdft-image
            pdft-notify
            pdft-media
            pdft-tui
            pdft-app)

# PDFT executable
add_subdirectory(cli)
