#include "pdft/core/assert.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft {

enum Mth {
    None,
    Default,
    Copy,
    CopyAssign,
    Move,
    MoveAssign,
    Destroyed,
};

struct X {
    ~X() {
        m = Destroyed;
    }
    X() : m(Default) {}
    X(const X&) : m(Copy) {}
    X& operator=(const X&) {
        m = CopyAssign;
        return *this;
    }
    X(X&&) noexcept : m(Move) {}
    X& operator=(X&&) noexcept {
        m = MoveAssign;
        return *this;
    }

    operator bool() const {
        return true;
    }

    Mth m = None;
};

TEST_CASE("verify/forward/temporary") {
    X x = verify(X{}, "test");
    CHECK(x.m == Move);
}

TEST_CASE("verify/forward/reference") {
    X y;
    X& x = verify(y, "test");
    CHECK(&x == &y);
}

TEST_CASE("verify/forward/xvalue") {
    X y;
    X x = verify(std::move(y), "test");
    CHECK(x.m == Move);
}

}  // namespace pdft
