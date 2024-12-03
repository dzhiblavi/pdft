#include "pdft/core/assert.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>

#include <catch2/catch_test_macros.hpp>

namespace pdft::tests {

template <typename F>
void runCoroutine(F func) {
    asio::io_context context;
    asio::co_spawn(context, func(context), asio::detached);

    try {
        context.run();
    } catch (...) {
        panic("unhandled exception");
    }
}

}  // namespace pdft::tests
