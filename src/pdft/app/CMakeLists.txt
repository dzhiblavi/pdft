find_package(asio REQUIRED)

add_library(
  pdft-app OBJECT
  ./app.cpp
  ./entrypoint.cpp
  ./palettes.cpp
  ./state/impl/app_ui.cpp
  ./state/document_storage.cpp
  ./state/palette_storage.cpp
  ./view/app_page_renderer.cpp
  ./view/app_view.cpp
  ./view/app_document_view.cpp
  ./view/app_image_renderer.cpp
  ./view/app_page_processor.cpp
  ./controller/keytree.cpp
  ./controller/app_controller.cpp
  ./loader/app_state_loader.cpp
  ./loader/app_persister.cpp
  ./loader/app_loader.cpp
  ./palettes.cpp)

target_link_libraries(pdft-app PUBLIC pdft-core pdft-tui asio::asio)
