#include "pdft/controller/keytree.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::controller {

TEST_CASE("testKeytreeEmpty", "") {
    [[maybe_unused]] KeyTree k;

    k.feed('0');
    k.feed('a');
    k.feed('1');
    k.feed('b');
    k.feed('2');
    k.feed('c');
}

TEST_CASE("testKeytreeSingleSimple", "") {
    KeyTree k;

    int a = 0;
    int b = 0;
    int c = 0;

    k.bind("a", [&] {
        ++a;
    });
    k.bind("b", [&] {
        ++b;
    });
    k.bind("c", [&] {
        ++c;
    });

    CHECK((a == 0 && b == 0 && c == 0));
    k.feed('b');
    CHECK((a == 0 && b == 1 && c == 0));
    k.feed('a');
    CHECK((a == 1 && b == 1 && c == 0));
    k.feed('a');
    CHECK((a == 2 && b == 1 && c == 0));
    k.feed('a');
    CHECK((a == 3 && b == 1 && c == 0));
    k.feed('b');
    CHECK((a == 3 && b == 2 && c == 0));
    k.feed('c');
    CHECK((a == 3 && b == 2 && c == 1));
}

TEST_CASE("testKeytreeMultiSimple", "") {
    KeyTree k;

    int a = 0;
    int b = 0;
    int c = 0;

    k.bind("aa", [&] {
        ++a;
    });
    k.bind("ab", [&] {
        ++b;
    });
    k.bind("ac", [&] {
        ++c;
    });

    CHECK((a == 0 && b == 0 && c == 0));
    k.feed('b');
    CHECK((a == 0 && b == 0 && c == 0));
    k.feed('a');
    CHECK((a == 0 && b == 0 && c == 0));
    k.feed('a');
    CHECK((a == 1 && b == 0 && c == 0));
    k.feed('a');
    CHECK((a == 1 && b == 0 && c == 0));
    k.feed('b');
    CHECK((a == 1 && b == 1 && c == 0));
    k.feed('c');
    CHECK((a == 1 && b == 1 && c == 0));
    k.feed('a');
    CHECK((a == 1 && b == 1 && c == 0));
    k.feed('c');
    CHECK((a == 1 && b == 1 && c == 1));
}

TEST_CASE("testKeytreeNumberOnly", "") {
    KeyTree k;

    int a = 0;
    int na = 0;

    int b = 0;
    int nb = 0;

    k.bind(KeyTree::number, "aa", [&](auto x) {
        ++a;
        na = x;
    });
    k.bind(KeyTree::number, "ab", [&](auto x) {
        ++b;
        nb = x;
    });

    auto check = [&](int ea, int ena, int eb, int enb) {
        CHECK(a == ea);
        CHECK(b == eb);
        CHECK(na == ena);
        CHECK(nb == enb);
        na = 0;
        nb = 0;
    };

    k.feed('a');
    k.feed('a');
    k.feed('0');
    k.feed('0');
    k.feed('1');
    k.feed('2');
    check(0, 0, 0, 0);
    k.feed('a');
    k.feed('a');
    check(1, 12, 0, 0);

    k.feed('a');
    k.feed('a');
    k.feed('0');
    k.feed('0');
    k.feed('2');
    k.feed('1');
    check(1, 0, 0, 0);
    k.feed('a');
    k.feed('b');
    check(1, 0, 1, 21);
}

TEST_CASE("testKeytreeMixed", "") {
    KeyTree k;

    int a = 0;
    int na = 0;
    int b = 0;

    k.bind(KeyTree::number, "aa", [&](auto n) {
        ++a;
        na = n;
    });
    k.bind("aa", [&] {
        ++b;
    });

    auto check = [&](int ea, int ena, int eb) {
        CHECK(a == ea);
        CHECK(na == ena);
        na = 0;
        CHECK(b == eb);
    };

    k.feed('a');
    check(0, 0, 0);
    k.feed('a');
    check(0, 0, 1);
    k.feed('a');
    check(0, 0, 1);
    k.feed('0');
    check(0, 0, 1);
    k.feed('b');
    check(0, 0, 1);
    k.feed('c');
    check(0, 0, 1);
    k.feed('1');
    k.feed('1');
    check(0, 0, 1);
    k.feed('a');
    check(0, 0, 1);
    k.feed('a');
    check(1, 11, 1);
}
TEST_CASE("testKeytreeFeedsAfterReset", "") {
    KeyTree k;

    int a = 0;
    int b = 0;

    k.bind("aaa", [&] {
        ++a;
    });
    k.bind("b", [&] {
        ++b;
    });

    k.feed('a');
    k.feed('a');
    CHECK((a == 0 && b == 0));

    k.feed('b');
    CHECK((a == 0 && b == 1));
}

TEST_CASE("testKeytreeFeedsAfterResetNumbers", "") {
    KeyTree k;

    int a = 0;
    int b = 0;

    k.bind(KeyTree::number, "a", [&](auto) {
        ++a;
    });
    k.bind("b", [&] {
        ++b;
    });

    k.feed('1');
    k.feed('0');
    CHECK((a == 0 && b == 0));

    k.feed('b');
    CHECK((a == 0 && b == 1));
}

}  // namespace pdft::controller
