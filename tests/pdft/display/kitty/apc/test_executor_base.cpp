#include "pdft/display/kitty/apc/executor_base.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::display::kitty::apc {

namespace {

struct TestExecutor : ExecutorBase {
    TestExecutor() : ExecutorBase({.max_chunk_size_bytes = 4}, nullptr) {}

 private:
    void writePrefix(core::io::OutBuffer& buffer) const override {
        core::io::write(buffer, "P");
    }

    void writeSuffix(core::io::OutBuffer& buffer) const override {
        core::io::write(buffer, "S");
    }
};

}  // namespace

TEST_CASE("testExecuteNoControlDataNoPayload", "") {
    core::io::OutBuffer buffer;
    auto executor = TestExecutor();

    executor.execute(
        {
            .command = "_G",
            .control_data = {},
        },
        buffer);

    CHECK(buffer.view() == "P_G;S");
}

TEST_CASE("testExecuteWithControlDataNoPayload", "") {
    core::io::OutBuffer buffer;
    auto executor = TestExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "P_Ga=p,C=1;S");
}

TEST_CASE("testExecuteNoControlDataWithPayloadOneChunk", "") {
    core::io::OutBuffer buffer;
    auto executor = TestExecutor();

    executor.execute(
        {
            .command = "_G",
            .payload = "abcd",  // max chunk size
            .control_data = {},
        },
        buffer);

    CHECK(buffer.view() == "P_G;abcdS");
}

TEST_CASE("testExecuteWithControlDataWithPayloadOneChunk", "") {
    core::io::OutBuffer buffer;
    auto executor = TestExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .payload = "abcd",  // max chunk size
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "P_Ga=p,C=1;abcdS");
}

TEST_CASE("testExecuteWithControlDataWithPayloadMultipleChunks", "") {
    core::io::OutBuffer buffer;
    auto executor = TestExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .payload = "abcdefgh012345",
            .control_data = control_data,
        },
        buffer);

    CHECK(buffer.view() == "P_Ga=p,C=1,m=1;abcdSP_Gm=1;efghSP_Gm=1;0123SP_Gm=0;45S");
}

TEST_CASE("testExecuteBase64EncodeSingleChunk", "") {
    core::io::OutBuffer buffer;
    auto executor = TestExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .payload = "+-=",
            .control_data = control_data,
            .b64_encode = true,
        },
        buffer);

    CHECK(buffer.view() == "P_Ga=p,C=1;Ky09S");
}

TEST_CASE("testExecuteBase64EncodeMultipleChunks", "") {
    core::io::OutBuffer buffer;
    auto executor = TestExecutor();

    std::pair<std::string_view, std::string> control_data[] = {
        {"a", "p"},
        {"C", "1"},
    };

    executor.execute(
        {
            .command = "_G",
            .payload = "+-=abcd%%%",
            .control_data = control_data,
            .b64_encode = true,
        },
        buffer);

    CHECK(buffer.view() == "P_Ga=p,C=1,m=1;Ky09SP_Gm=1;YWJjSP_Gm=1;ZCUlSP_Gm=0;JQ==S");
}

}  // namespace pdft::display::kitty::apc
