#include "pdft/utils/cli/cli.h"
#include "pdft/utils/cli/detail/parsed.h"
#include "pdft/utils/cli/detail/register.h"
#include "pdft/utils/cli/detail/storage.h"

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/type/maybe.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::cli::detail {

enum class E {
    A,
    B,
};

template <std::same_as<const char*>... Args>
void parse(OptionsStorage& storage, Args... args) {
    TCLAP::CmdLine cmd("", ' ', "");
    cmd.setExceptionHandling(false);
    storage.attach(cmd);
    cmd.parse(sizeof...(Args) + 1, std::array{"", args...}.data());
}

template <typename T, typename F, std::same_as<const char*>... Args>
void test(F check, Args... args) {
    CliArgsFor<T> cli_args = makeCliArgs<T>();
    OptionsStorage options;
    addOptions<T>(options, cli_args);

    try {
        parse(options, args...);
        check(parsed<T>(cli_args));
    } catch (...) {
        check(Result<T>(result::err("error")));
    }
}

TEST_CASE("testParseFromCliArgsMissing") {
    struct X {
        opt<int, 'x', "xxx"> x = 0;
    };

    test<X>([](auto x) {
        CHECK(x);
        CHECK(x->x == 0);
    });
}

TEST_CASE("testParseFromCliArgsShortPresent") {
    struct X {
        opt<int, 'x', "xxx"> xyx = 0;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->xyx == 10);
        },
        "-x",
        "10");
}

TEST_CASE("testParseFromCliArgsLongPresentFieldName") {
    struct X {
        opt<int, 'x'> xyx = 0;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->xyx == 10);
        },
        "--xyx",
        "10");
}

TEST_CASE("testParseFromCliArgsLongPresentCustomName") {
    struct X {
        opt<int, 'x', "", "xxx"> xyx = 0;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->xyx == 10);
        },
        "--xxx",
        "10");
}

TEST_CASE("testParseFromCliArgsIntInvalid") {
    struct X {
        opt<int, 'x'> xyx = 0;
    };

    test<X>([](auto x) { CHECK(!x); }, "-x", "abacaba");
}

TEST_CASE("testParseFromCliArgsMaybeIntMissing") {
    struct X {
        opt<Maybe<int>, 'x'> x;
    };

    test<X>([](auto x) {
        CHECK(x);
        CHECK(!x->x->has_value());
    });
}

TEST_CASE("testParseFromCliArgsMaybeIntSome") {
    struct X {
        opt<Maybe<int>, 'x'> x = 0;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->x->has_value());
            CHECK(x->x->value() == 10);
        },
        "-x",
        "10");
}

TEST_CASE("testParseFromCliArgsMaybeIntInvalid") {
    struct X {
        opt<Maybe<int>, 'x'> x = 0;
    };

    test<X>([](auto x) { CHECK(!x); }, "-x", "abacaba");
}

TEST_CASE("testParseFromCliArgsEnumMissing") {
    struct X {
        opt<E, 'e'> e = E::A;
    };

    test<X>([](auto x) {
        CHECK(x);
        CHECK(x->e == E::A);
    });
}

TEST_CASE("testParseFromCliArgsEnumSome") {
    struct X {
        opt<E, 'e'> e = E::A;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->e == E::B);
        },
        "-e",
        "B");
}

TEST_CASE("testParseFromCliArgsEnumInvalid") {
    struct X {
        opt<E, 'e'> e = E::A;
    };

    test<X>([](auto x) { CHECK(!x); }, "-e", "X");
}

TEST_CASE("testParseFromCliArgsMaybeEnumMissing") {
    struct X {
        opt<Maybe<E>, 'e'> e;
    };

    test<X>([](auto x) {
        CHECK(x);
        CHECK(!x->e->has_value());
    });
}

TEST_CASE("testParseFromCliArgsMaybeEnumSome") {
    struct X {
        opt<Maybe<E>, 'e'> e;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->e->has_value());
            CHECK(x->e->value() == E::A);
        },
        "-e",
        "A");
}

TEST_CASE("testParseFromCliArgsMaybeEnumInvalid") {
    struct X {
        opt<Maybe<E>, 'e'> e;
    };

    test<X>([](auto x) { CHECK(!x); }, "-e", "X");
}

TEST_CASE("testParseFromCliArgsIntVectorNone") {
    struct X {
        opt<std::vector<int>, 'v'> v;
    };

    test<X>([](auto x) {
        CHECK(x);
        CHECK(x->v->empty());
    });
}

TEST_CASE("testParseFromCliArgsIntVectorOne") {
    struct X {
        opt<std::vector<int>, 'v'> v;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->v->size() == 1);
            CHECK(x->v->at(0) == 123);
        },
        "-v",
        "123");
}

TEST_CASE("testParseFromCliArgsIntVectorMany") {
    struct X {
        opt<std::vector<int>, 'v'> v;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->v->size() == 2);
            CHECK(x->v->at(0) == 123);
            CHECK(x->v->at(1) == 456);
        },
        "-v",
        "123",
        "-v",
        "456");
}

TEST_CASE("testParseFromCliArgsIntVectorInvalid") {
    struct X {
        opt<std::vector<int>, 'v'> v;
    };

    test<X>([](auto x) { CHECK(!x); }, "-v", "123", "-v", "abacaba");
}

TEST_CASE("testParseFromCliArgsEnumVectorOne") {
    struct X {
        opt<std::vector<E>, 'v'> v;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->v->size() == 1);
            CHECK(x->v->at(0) == E::A);
        },
        "-v",
        "A");
}

TEST_CASE("testParseFromCliArgsEnumVectorMany") {
    struct X {
        opt<std::vector<E>, 'v'> v;
    };

    test<X>(
        [](auto x) {
            CHECK(x);
            CHECK(x->v->size() == 2);
            CHECK(x->v->at(0) == E::B);
            CHECK(x->v->at(1) == E::A);
        },
        "-v",
        "B",
        "-v",
        "A");
}

TEST_CASE("testParseFromCliArgsEnumVectorInvalid") {
    struct X {
        opt<std::vector<E>, 'v'> v;
    };

    test<X>([](auto x) { CHECK(!x); }, "-v", "B", "-v", "X");
}

}  // namespace pdft::cli::detail
