#pragma once

#include <random>

namespace pdft::utils {

std::mt19937_64& threadLocalGenerator();

template <std::integral T>
T random(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) {
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(threadLocalGenerator());
}

}  // namespace pdft::utils
