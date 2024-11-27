#include "pdft/image/image.h"

namespace pdft::image {

void invertColors(Image& image) {
    for (uint8_t& byte : image.mutableSamples()) {
        byte = 255 - byte;
    }
}

}  // namespace pdft::image
