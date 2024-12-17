add_library(
  pdft-media OBJECT ./mupdf/context.cpp ./mupdf/document.cpp ./mupdf/page.cpp
                    ./mupdf/pixmap_image.cpp ./mupdf/loader.cpp)

target_link_libraries(pdft-media PUBLIC pdft-core libmupdf_ext)
