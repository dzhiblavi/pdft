#pragma once

namespace pdft::utils {

struct Pinned {
    Pinned() = default;
    Pinned(const Pinned&) = delete;
    Pinned& operator=(const Pinned&) = delete;
    Pinned(Pinned&&) = delete;
    Pinned& operator=(Pinned&&) = delete;
};

}  // namespace pdft::utils
