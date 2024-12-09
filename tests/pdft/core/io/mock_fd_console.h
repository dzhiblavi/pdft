#include "pdft/core/io/console_reader.h"
#include "pdft/core/io/console_writer.h"
#include "pdft/core/rc.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <format>
#include <iostream>
#include <unistd.h>

namespace pdft::io::test {

namespace detail {

inline void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

struct FileDescriptor {
    explicit FileDescriptor(int fd) : fd(fd) {}

    ~FileDescriptor() {
        if (fd != -1) {
            close(fd);
        }
    }

    FileDescriptor(FileDescriptor&& other) : fd(std::exchange(other.fd, -1)) {}

    void write(std::string_view s) {
        std::cout << std::format("call write with '{}'", s) << std::endl;
        auto write = ::write(fd, s.data(), s.size());
        std::cout << std::format("write to {}: {}, wanted {}", fd, write, s.size()) << std::endl;
    }

    std::string read(size_t max_bytes) {
        std::string buf;
        buf.resize(max_bytes + 1);
        std::cout << std::format("call read max_bytes={}", max_bytes) << std::endl;
        auto read = ::read(fd, buf.data(), max_bytes);
        buf[read] = '\0';
        buf.resize(read);
        return std::string(buf.data());
    }

    int fd = -1;
};

}  // namespace detail

struct MockConsole : ConsoleWriter, ConsoleReader {
    MockConsole(int in_fd[2], int out_fd[2])
        : in_fd_r(in_fd[0])
        , in_fd_w(in_fd[1])
        , out_fd_r(out_fd[0])
        , out_fd_w(out_fd[1]) {}

    using ConsoleReader::read;
    using ConsoleWriter::write;

    AsyncStatus write(std::string_view bytes) override {
        out_fd_w.write(bytes);
        co_return result::ok();
    }

    AsyncStatus writeMany(std::vector<asio::const_buffer> buffers) override {
        for (const auto& s : buffers) {
            out_fd_w.write({(const char*)s.data(), s.size()});
        }
        co_return result::ok();
    }

    AsyncStatus read(size_t num_bytes, std::string& out) override {
        out = in_fd_r.read(num_bytes);
        co_return result::ok();
    }

    void put(std::string s) {
        in_fd_w.write(std::move(s));
    }

    std::string written(size_t max_bytes = 100) {
        return out_fd_r.read(max_bytes);
    }

    int inFd() {
        return in_fd_r.fd;
    }

    int outFd() {
        return out_fd_w.fd;
    }

    detail::FileDescriptor in_fd_r, in_fd_w, out_fd_r, out_fd_w;
};

inline Arc<MockConsole> makeMockConsole() {
    int in_fds[2];
    (void)pipe(in_fds);

    int out_fds[2];
    (void)pipe(out_fds);

    detail::setNonBlocking(in_fds[0]);
    detail::setNonBlocking(in_fds[1]);
    detail::setNonBlocking(out_fds[0]);
    detail::setNonBlocking(out_fds[1]);

    return std::make_shared<MockConsole>(in_fds, out_fds);
}

}  // namespace pdft::io::test
