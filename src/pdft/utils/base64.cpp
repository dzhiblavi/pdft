#include "pdft/utils/base64.h"

#include <libbase64.h>

namespace pdft::utils {

std::string encodeBase64(std::string_view data) {
    size_t size = (data.size() + 2) / 3 * 4;

    std::string buf;
    buf.resize(size + 1);  // for trailing \0 character

    base64_encode(data.data(), data.size(), buf.data(), &size, 0);
    buf[size] = '\0';
    buf.resize(size);

    return buf;
}

}  // namespace pdft::utils
