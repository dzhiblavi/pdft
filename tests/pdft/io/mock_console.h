#include "pdft/io/console_reader.h"
#include "pdft/io/console_writer.h"

#include "pdft/core/rc.h"

#include <format>
#include <iostream>

namespace pdft::io::test {

struct MockConsole : ConsoleWriter, ConsoleReader {
    using ConsoleReader::read;
    using ConsoleWriter::write;

    AsyncStatus write(std::string_view bytes) override {
        std::cerr << std::format("write '{}' to console", bytes) << std::endl;
        out_ += bytes;
        co_return result::ok();
    }

    AsyncStatus writeMany(std::vector<asio::const_buffer> buffers) override {
        for (const auto& buf : buffers) {
            out_ += std::string_view{(const char*)buf.data(), buf.size()};
        }
        co_return result::ok();
    }

    AsyncStatus read(size_t num_bytes, std::string& out) override {
        auto result = readFrom(in_, num_bytes);
        std::cerr << std::format("read '{}' from console (requested {} bytes)", result, num_bytes)
                  << std::endl;
        out = result;
        co_return result::ok();
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

inline Arc<MockConsole> makeMockConsole() {
    return std::make_shared<MockConsole>();
}

}  // namespace pdft::io::test
