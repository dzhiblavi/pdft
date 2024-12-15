#include "pdft/utils/cli/cli.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::cli::detail {

TEST_CASE("testDefaultSpecification") {
    struct X {
        opt<int, 'x', "xxx"> x = 0;
    };


}

}  // namespace pdft::cli::detail
