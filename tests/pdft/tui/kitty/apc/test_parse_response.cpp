#include "pdft/tui/kitty/apc/parse_response.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::tui {

TEST_CASE("parseResponse/empty") {
    auto resp = parseResponse("");
    CHECK(!resp.has_value());
}

TEST_CASE("parseResponse/no-first-escape") {
    auto resp = parseResponse("_Ga=b;\033");
    CHECK(!resp.has_value());
}

TEST_CASE("parseResponse/no-last-escape") {
    auto resp = parseResponse("\033_Ga=b;\033b");
    CHECK(!resp.has_value());
}

TEST_CASE("parseResponse/no-control/no-payload") {
    auto resp = parseResponse("\033_G;\033\\");

    CHECK(resp.has_value());
    auto&& apc = resp.value();

    CHECK(apc.command == "_G");
    CHECK(apc.payload.empty());
    CHECK(apc.control_data.empty());
}

TEST_CASE("parseResponse/no-payload-delimiter") {
    auto resp = parseResponse("\033_Ga=b\033\\");
    CHECK(!resp.has_value());
}

TEST_CASE("parseResponse/control/no-payload") {
    auto resp = parseResponse("\033_Ga=b;\033\\");

    CHECK(resp.has_value());
    auto&& apc = resp.value();

    CHECK(apc.command == "_G");
    CHECK(apc.payload.empty());
    CHECK(apc.control_data.size() == 1);
    CHECK(apc.control_data[0] == std::make_pair("a", "b"));
}

TEST_CASE("parseResponse/control/payload") {
    auto resp = parseResponse("\033_Ga=b;the payload is [OK]\033\\");

    CHECK(resp.has_value());
    auto&& apc = resp.value();

    CHECK(apc.command == "_G");
    CHECK(apc.payload == "the payload is [OK]");
    CHECK(apc.control_data.size() == 1);
    CHECK(apc.control_data[0] == std::make_pair("a", "b"));
}

TEST_CASE("parseResponse/multi-control/payload") {
    auto resp = parseResponse("\033_Ga=b,c=d,a_b_'=hello world;\033\\");

    CHECK(resp.has_value());
    auto&& apc = resp.value();

    CHECK(apc.command == "_G");
    CHECK(apc.payload.empty());
    CHECK(apc.control_data.size() == 3);
    CHECK(apc.control_data[0] == std::make_pair("a", "b"));
    CHECK(apc.control_data[1] == std::make_pair("c", "d"));
    CHECK(apc.control_data[2] == std::make_pair("a_b_'", "hello world"));
}

TEST_CASE("parseResponse/invalid-control-data-empty") {
    auto resp = parseResponse("\033_G,,,,;\033\\");
    CHECK(!resp.has_value());
}

TEST_CASE("parseResponse/invalid-control-data-not-a-kv") {
    auto resp = parseResponse("\033_GAB;\033\\");
    CHECK(!resp.has_value());
}

}  // namespace pdft::tui
