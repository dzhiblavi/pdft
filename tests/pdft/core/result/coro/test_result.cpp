#include "pdft/core/result.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft {

Result<int> just(int x) {
    co_return x;
}

Result<int> faulty(std::string error) {
    return result::err(error);
}

TEST_CASE("testMaybeResultOk") {
    auto x = [] -> Result<int> {
        int x = co_await just(10);
        int y = co_await just(20);
        co_return x + y;
    }();

    CHECK(x.has_value());
    CHECK(*x == 30);
}

TEST_CASE("testMaybeResultFaulty") {
    bool reached = false;

    auto x = [&reached] -> Result<int> {
        int x = co_await just(10);
        int y = co_await just(20);
        reached = true;
        co_return x + y + co_await faulty("an error");
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == "an error");
    CHECK(reached);
}

TEST_CASE("testMaybeResultFaultyMidWay") {
    int count = 0;

    auto x = [&count] -> Result<int> {
        int x = co_await just(10);
        ++count;
        int y = co_await faulty("an error");
        ++count;
        co_return x + y;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == "an error");
    CHECK(count == 1);
}

TEST_CASE("testMaybeResultConvertError") {
    int count = 0;

    auto x = [&count] -> Result<int> {
        int x = co_await just(10);
        ++count;
        int y = co_await (faulty("kek") | result::mapErr([](auto err) { return "error: " + err; }));
        ++count;
        co_return x + y;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == "error: kek");
    CHECK(count == 1);
}

}  // namespace pdft
