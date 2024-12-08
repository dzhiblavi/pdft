#pragma once

#include <future>
#include <saf.hpp>

namespace pdft {

template <typename T>
using Future = std::future<T>;
// using Future = saf::cc_future<T>;

template <typename T>
using Promise = std::promise<T>;
// using Promise = saf::cc_promise<T>;

}  // namespace pdft
