#pragma once

#include "pdft/core/rc.h"

namespace pdft {

template <typename I>
class AbstractFactory {
 public:
    virtual ~AbstractFactory() = default;
    virtual Arc<I> create() const = 0;
};

template <typename F, typename I>
concept IsFactory = std::derived_from<F, AbstractFactory<I>>;

}  // namespace pdft
