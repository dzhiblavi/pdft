#include "pdft/core/io/console.h"

#include <format>
#include <iostream>

namespace pdft::core::io::test {

struct MockConsole : Console {
    asio::awaitable<void> write(std::string_view bytes) override {
        std::cerr << std::format("write '{}' to console", bytes) << std::endl;
        out_ += bytes;
        co_return;
    }

    asio::awaitable<std::string> read(size_t num_bytes) override {
        auto result = readFrom(in_, num_bytes);
        std::cerr << std::format("read '{}' from console (requested {} bytes)", result, num_bytes)
                  << std::endl;
        co_return result;
    }

    void put(std::string s) {
        in_ += s;
    }

    std::string written(size_t max_bytes = 100) {
        return readFrom(out_, max_bytes);
    }

 private:
    std::string readFrom(std::string& in, size_t num_bytes) {
        if (in.size() < num_bytes) {
            auto result = in;
            in = "";
            return result;
        }

        auto result = in.substr(0, num_bytes);
        in = in.substr(num_bytes);
        return result;
    }

    std::string in_;
    std::string out_;
};

std::shared_ptr<MockConsole> makeMockConsole(asio::io_context& context) {
    return std::make_shared<MockConsole>();
}

}  // namespace pdft::core::io::test
