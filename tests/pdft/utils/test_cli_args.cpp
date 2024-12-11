#include "pdft/utils/cli_args/cli_args.h"

#include "pdft/core/result/type/maybe.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::utils {

enum class E {
    A,
    B,
};

template <std::same_as<const char*>... Args>
std::array<const char*, sizeof...(Args) + 1> args(Args... args) {
    return {"", args...};
}

TEST_CASE("testParseFromCliArgsInt") {
    struct X {
        int x = 0;
    };

    auto arr = args("-x", "10");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->x == 10);
}

TEST_CASE("testParseFromCliArgsMaybeInt") {
    struct X {
        Maybe<int> x = 0;
    };

    auto arr = args("-x", "10");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->x == 10);
}

TEST_CASE("testParseFromCliArgsIntMissing") {
    struct X {
        int x = 0;
    };

    auto arr = args("-y", "10");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->x == 0);
}

TEST_CASE("testParseFromCliArgsMaybeIntMissing") {
    struct X {
        Maybe<int> x = 0;
    };

    auto arr = args("-y", "10");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->x == 0);
}

TEST_CASE("testParseFromCliArgsEnum") {
    enum class E {
        A,
        B,
    };

    struct X {
        E en = E::B;
    };

    auto arr = args("--en", "A");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->en == E::A);
}

TEST_CASE("testParseFromCliArgsEnumMissing") {
    struct X {
        E en = E::B;
    };

    auto arr = args("--eng", "A");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->en == E::B);
}

TEST_CASE("testParseFromCliArgsEnumBad") {
    struct X {
        E en = E::B;
    };

    auto arr = args("--en", "D");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(!x.has_value());
}

TEST_CASE("testParseFromCliArgsMaybeEnum") {
    struct X {
        Maybe<E> en;
    };

    auto arr = args("--en", "A");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->en == E::A);
}

TEST_CASE("testParseFromCliArgsMaybeEnumMissing") {
    struct X {
        Maybe<E> en;
    };

    auto arr = args("--eng", "A");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(!x->en.has_value());
}

TEST_CASE("testParseFromCliArgsVectorOfPrimitives") {
    struct X {
        std::vector<int> v;
    };

    auto arr = args("--v", "10", "--v", "20");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->v == std::vector{10, 20});
}

TEST_CASE("testParseFromCliArgsVectorOfPrimitivesMissing") {
    struct X {
        std::vector<int> v;
    };

    auto arr = args("--y", "10", "--y", "20");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->v == std::vector<int>{});
}

TEST_CASE("testParseFromCliArgsVectorOfEnums") {
    struct X {
        std::vector<E> v;
    };

    auto arr = args("--v", "B", "--v", "A");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->v == std::vector{E::B, E::A});
}

TEST_CASE("testParseFromCliArgsVectorOfEnumsMissing") {
    struct X {
        std::vector<E> v;
    };

    auto arr = args("--y", "10", "--y", "20");
    auto x = parseFromCliArgs<X>(arr);

    CHECK(x.has_value());
    CHECK(x->v == std::vector<E>{});
}

}  // namespace pdft::utils
