#include "pdft/core/result.h"
#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/coro/result.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft {

using namespace result;

Result<int> just(int x) {
    co_return x;
}

Result<int> faulty(std::errc value) {
    return result::err(std::make_error_code(value));
}

TEST_CASE("testAndThenResultErr") {
    auto e = std::make_error_code(std::errc::io_error);
    auto x = Result<int>{err(e)} | andThen([](int) { return ok(10); });
    CHECK(!x.has_value());
    CHECK(x.error() == e);
}

TEST_CASE("testAndThenResultOk") {
    auto x = ok(10) | andThen([](int x) { return ok(x * 2); });
    CHECK(x.has_value());
    CHECK(*x == 20);
}

TEST_CASE("testMapResultErr") {
    auto e = std::make_error_code(std::errc::io_error);
    auto x = Result<int>(err(e)) | map([](int) { return 10; });
    CHECK(!x.has_value());
}

TEST_CASE("testMapResultOk") {
    auto x = ok(10) | map([](int x) { return x * 2; });
    CHECK(x.has_value());
    CHECK(*x == 20);
}

TEST_CASE("testMapErrErr") {
    auto e = std::make_error_code(std::errc::io_error);
    auto x = Result<int>(err(e)) |
             mapErr([](auto) { return std::make_error_code(std::errc::executable_format_error); });
    CHECK(!x.has_value());
    CHECK(x.error() == std::make_error_code(std::errc::executable_format_error));
}

TEST_CASE("testMapErrOk") {
    auto x = ok(10) |
             mapErr([](auto) { return std::make_error_code(std::errc::executable_format_error); });
    CHECK(x.has_value());
    CHECK(*x == 10);
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
        int z = co_await faulty(std::errc::io_error);
        ++count;
        co_return x + y + z;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == std::make_error_code(std::errc::io_error));
    CHECK(count == 2);
}

TEST_CASE("testCoroResultFaultyMidWay") {
    int count = 0;

    auto x = [&count] -> Result<int> {
        int x = co_await just(10);
        ++count;
        int y = co_await faulty(std::errc::io_error);
        ++count;
        co_return x + y;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == std::make_error_code(std::errc::io_error));
    CHECK(count == 1);
}

TEST_CASE("testCoroResultConvertError") {
    int count = 0;

    auto x = [&count] -> Result<int> {
        int x = co_await just(10);
        ++count;
        int y = co_await (faulty(std::errc::io_error) | result::mapErr([](auto) {
                              return std::make_error_code(std::errc::executable_format_error);
                          }));
        ++count;
        co_return x + y;
    }();

    CHECK(!x.has_value());
    CHECK(x.error() == std::make_error_code(std::errc::executable_format_error));
    CHECK(count == 1);
}

Maybe<int> get_int() {
    co_return 10;
}

Result<Unit> aunit() {
    co_return;
}

TEST_CASE("testCoroResultConvertUnit") {
    int z = 0;

    auto x = [&] -> Result<Unit> {
        z = co_await (get_int() | result::lift(std::make_error_code(std::errc::io_error)));
        co_await aunit();
        co_return;
    }();

    CHECK(x.has_value());
    CHECK(z == 10);
}

}  // namespace pdft
