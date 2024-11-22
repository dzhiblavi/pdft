#include "pdft/core/apc/parse_response.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::core::apc {

TEST_CASE("testParseResponseEmpty", "") {
    auto resp = parseResponse("");

    CHECK(!resp.has_value());
    CHECK(resp.error() == "too short APC response to be valid");
}

TEST_CASE("testParseResponseNoFirstEscapeCode", "") {
    auto resp = parseResponse("_Ga=b;\033");

    CHECK(!resp.has_value());
    CHECK(resp.error() == "APC response does not start with the escape code");
}

TEST_CASE("testParseResponseNoLastEscapeCode", "") {
    auto resp = parseResponse("\033_Ga=b;\033b");

    CHECK(!resp.has_value());
    CHECK(resp.error() == "APC response does not end with the escape code");
}

TEST_CASE("testParseResponseNoControlDataNoPayload", "") {
    auto resp = parseResponse("\033_G;\033");

    CHECK(resp.has_value());
    auto&& apc = resp.value();

    CHECK(apc.command == "_G");
    CHECK(apc.payload.empty());
    CHECK(apc.control_data.empty());
}

TEST_CASE("testParseResponseNoPayloadDelimiter", "") {
    auto resp = parseResponse("\033_Ga=b\033");

    CHECK(!resp.has_value());
    CHECK(resp.error() == "command _G did not terminate its control data with ;");
}

TEST_CASE("testParseResponseSingleControlDataNoPayload", "") {
    auto resp = parseResponse("\033_Ga=b;\033");

    CHECK(resp.has_value());
    auto&& apc = resp.value();

    CHECK(apc.command == "_G");
    CHECK(apc.payload.empty());
    CHECK(apc.control_data.size() == 1);
    CHECK(apc.control_data[0] == std::make_pair("a", "b"));
}

TEST_CASE("testParseResponseSingleControlDataWithPayload", "") {
    auto resp = parseResponse("\033_Ga=b;the payload is [OK]\033");

    CHECK(resp.has_value());
    auto&& apc = resp.value();

    CHECK(apc.command == "_G");
    CHECK(apc.payload == "the payload is [OK]");
    CHECK(apc.control_data.size() == 1);
    CHECK(apc.control_data[0] == std::make_pair("a", "b"));
}

TEST_CASE("testParseResponseMultipleControlDataNoPayload", "") {
    auto resp = parseResponse("\033_Ga=b,c=d,a_b_'=hello world;\033");

    CHECK(resp.has_value());
    auto&& apc = resp.value();

    CHECK(apc.command == "_G");
    CHECK(apc.payload.empty());
    CHECK(apc.control_data.size() == 3);
    CHECK(apc.control_data[0] == std::make_pair("a", "b"));
    CHECK(apc.control_data[1] == std::make_pair("c", "d"));
    CHECK(apc.control_data[2] == std::make_pair("a_b_'", "hello world"));
}

TEST_CASE("testParseResponseInvalidControlDataEmpty", "") {
    auto resp = parseResponse("\033_G,,,,;\033");

    CHECK(!resp.has_value());
    CHECK(resp.error() == "malformed key-value control data item: ");
}

TEST_CASE("testParseResponseInvalidControlDataNotAKeyValue", "") {
    auto resp = parseResponse("\033_GAB;\033");

    CHECK(!resp.has_value());
    CHECK(resp.error() == "malformed key-value control data item: AB");
}

}  // namespace pdft::core::apc
