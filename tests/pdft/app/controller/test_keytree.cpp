#include "pdft/app/controller/keytree.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::app::controller {

using enum KeyTree::Result;

TEST_CASE("testKeytreeEmpty", "") {
    KeyTree k;

    CHECK(k.feed('0') == Rejected);
    CHECK(k.feed('a') == Rejected);
    CHECK(k.feed('1') == Rejected);
    CHECK(k.feed('b') == Rejected);
    CHECK(k.feed('2') == Rejected);
    CHECK(k.feed('c') == Rejected);
}

TEST_CASE("testKeytreeSingleSimple", "") {
    KeyTree k;

    int a = 0;
    int b = 0;
    int c = 0;

    k.bind("a", [&] { ++a; });
    k.bind("b", [&] { ++b; });
    k.bind("c", [&] { ++c; });

    CHECK((a == 0 && b == 0 && c == 0));
    CHECK(k.feed('b') == Terminated);
    CHECK((a == 0 && b == 1 && c == 0));
    CHECK(k.feed('a') == Terminated);
    CHECK((a == 1 && b == 1 && c == 0));
    CHECK(k.feed('a') == Terminated);
    CHECK((a == 2 && b == 1 && c == 0));
    CHECK(k.feed('a') == Terminated);
    CHECK((a == 3 && b == 1 && c == 0));
    CHECK(k.feed('b') == Terminated);
    CHECK((a == 3 && b == 2 && c == 0));
    CHECK(k.feed('c') == Terminated);
    CHECK((a == 3 && b == 2 && c == 1));
}

TEST_CASE("testKeytreeMultiSimple", "") {
    KeyTree k;

    int a = 0;
    int b = 0;
    int c = 0;

    k.bind("aa", [&] { ++a; });
    k.bind("ab", [&] { ++b; });
    k.bind("ac", [&] { ++c; });

    CHECK((a == 0 && b == 0 && c == 0));
    CHECK(k.feed('b') == Rejected);
    CHECK((a == 0 && b == 0 && c == 0));
    CHECK(k.feed('a') == Accepted);
    CHECK((a == 0 && b == 0 && c == 0));
    CHECK(k.feed('a') == Terminated);
    CHECK((a == 1 && b == 0 && c == 0));
    CHECK(k.feed('a') == Accepted);
    CHECK((a == 1 && b == 0 && c == 0));
    CHECK(k.feed('b') == Terminated);
    CHECK((a == 1 && b == 1 && c == 0));
    CHECK(k.feed('c') == Rejected);
    CHECK((a == 1 && b == 1 && c == 0));
    CHECK(k.feed('a') == Accepted);
    CHECK((a == 1 && b == 1 && c == 0));
    CHECK(k.feed('c') == Terminated);
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

    CHECK(k.feed('a') == Rejected);
    CHECK(k.feed('a') == Rejected);
    CHECK(k.feed('0') == Accepted);
    CHECK(k.feed('0') == Accepted);
    CHECK(k.feed('1') == Accepted);
    CHECK(k.feed('2') == Accepted);
    check(0, 0, 0, 0);
    CHECK(k.feed('a') == Accepted);
    CHECK(k.feed('a') == Terminated);
    check(1, 12, 0, 0);

    CHECK(k.feed('a') == Rejected);
    CHECK(k.feed('a') == Rejected);
    CHECK(k.feed('0') == Accepted);
    CHECK(k.feed('0') == Accepted);
    CHECK(k.feed('2') == Accepted);
    CHECK(k.feed('1') == Accepted);
    check(1, 0, 0, 0);
    CHECK(k.feed('a') == Accepted);
    CHECK(k.feed('b') == Terminated);
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
    k.bind("aa", [&] { ++b; });

    auto check = [&](int ea, int ena, int eb) {
        CHECK(a == ea);
        CHECK(na == ena);
        na = 0;
        CHECK(b == eb);
    };

    CHECK(k.feed('a') == Accepted);
    check(0, 0, 0);
    CHECK(k.feed('a') == Terminated);
    check(0, 0, 1);
    CHECK(k.feed('a') == Accepted);
    check(0, 0, 1);
    CHECK(k.feed('0') == Rejected);
    check(0, 0, 1);

    k.reset();
    CHECK(k.feed('b') == Rejected);
    check(0, 0, 1);
    CHECK(k.feed('c') == Rejected);
    check(0, 0, 1);
    CHECK(k.feed('1') == Accepted);
    CHECK(k.feed('1') == Accepted);
    check(0, 0, 1);
    CHECK(k.feed('a') == Accepted);
    check(0, 0, 1);
    CHECK(k.feed('a') == Terminated);
    check(1, 11, 1);
}

TEST_CASE("testKeytreeFeedAfterReset", "") {
    KeyTree k;

    int a = 0;
    int b = 0;

    k.bind("aaa", [&] { ++a; });
    k.bind("b", [&] { ++b; });

    CHECK(k.feed('a') == Accepted);
    CHECK(k.feed('a') == Accepted);
    CHECK((a == 0 && b == 0));

    CHECK(k.feed('b') == Rejected);
    k.reset();

    CHECK(k.feed('b') == Terminated);
    CHECK((a == 0 && b == 1));
}

TEST_CASE("testKeytreeFeedsAfterResetNumbers", "") {
    KeyTree k;

    int a = 0;
    int b = 0;

    k.bind(KeyTree::number, "a", [&](auto) { ++a; });
    k.bind("b", [&] { ++b; });

    CHECK(k.feed('1') == Accepted);
    CHECK(k.feed('0') == Accepted);
    CHECK((a == 0 && b == 0));

    CHECK(k.feed('b') == Rejected);
    CHECK((a == 0 && b == 0));

    k.reset();
    CHECK(k.feed('b') == Terminated);
    CHECK((a == 0 && b == 1));
}

}  // namespace pdft::app::controller
