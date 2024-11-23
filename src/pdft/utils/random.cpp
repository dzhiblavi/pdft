#include "pdft/utils/random.h"

namespace pdft::utils {

std::mt19937_64& threadLocalGenerator() {
    thread_local std::mt19937_64 gen = [] {
        std::random_device rd;
        return std::mt19937_64(rd());
    }();

    return gen;
}

}  // namespace pdft::utils
