#include "pdft/utils/zlib.h"
#include "pdft/core/assert.h"

#include <zlib.h>

namespace pdft::utils {

std::string zlibCompress(std::string_view data) {
    auto bound = compressBound(data.size());

    std::string compressed;
    compressed.resize(1 + bound);

    unsigned long size = bound;
    auto errc = compress(
        reinterpret_cast<unsigned char*>(compressed.data()),  // NOLINT
        &size,
        reinterpret_cast<const unsigned char*>(data.data()),  // NOLINT
        data.size());

    PDFT_VERIFY(errc == Z_OK, "failed to compress data with zlib, errc={}", errc);
    compressed.resize(size);
    return compressed;
}

}  // namespace pdft::utils
