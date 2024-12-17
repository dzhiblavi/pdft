#include "pdft/utils/zlib.h"

#include <cassert>
#include <zlib.h>

namespace pdft::utils {

std::string zlibCompress(std::string_view data) {
    auto bound = compressBound(data.size());

    std::string compressed;
    compressed.resize(1 + bound);

    unsigned long size = bound;
    [[maybe_unused]] auto errc = compress(
        reinterpret_cast<unsigned char*>(compressed.data()),  // NOLINT
        &size,
        reinterpret_cast<const unsigned char*>(data.data()),  // NOLINT
        data.size());

    assert(errc == Z_OK);
    compressed.resize(size);
    return compressed;
}

}  // namespace pdft::utils
