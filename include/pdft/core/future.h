#pragma once

#include <saf.hpp>

namespace pdft {

template <typename T>
using Future = saf::cc_future<T>;

template <typename T>
using Promise = saf::cc_promise<T>;

}  // namespace pdft
