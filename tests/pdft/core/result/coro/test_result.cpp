#include "pdft/core/result.h"
#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/coro/result.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft {

Result<int> just(int x) {
    co_return x;
}

Result<int> faulty(std::string error) {
    return result::err(error);
}

TEST_CASE("testCoroResultOk") {
    auto x = [] -> Result<int> {
        int x = co_await just(10);
        int y = co_await just(20);
        co_return x + y;
    }();

    CHECK(x.has_value());
    CHECK(*x == 30);
}

TEST_CASE("testCoroResultFaulty") {
    int count = 0;

    auto x = [&count] -> Result<int> {
        int x = co_await just(10);
        ++count;
        int y = co_await just(20);
        ++count;
        int z = co_await faulty("an error");
        ++count;
        co_return x + y + z;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == "an error");
    CHECK(count == 2);
}

TEST_CASE("testCoroResultFaultyMidWay") {
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

TEST_CASE("testCoroResultConvertError") {
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

Maybe<int> get_int() {
    co_return 10;
}

Result<Unit> aunit() {
    co_return unit;
}

TEST_CASE("testCoroResultConvertUnit") {
    int z = 0;

    auto x = [&] -> Result<Unit> {
        z = co_await (get_int() | result::lift("hello"));
        co_await aunit();
        co_return unit;
    }();

    CHECK(x.has_value());
    CHECK(z == 10);
}

}  // namespace pdft
