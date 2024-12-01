#pragma once

#include "pdft/core/assert.h"

#include <array>
#include <cstddef>

namespace pdft::utils {

template <typename T, size_t N, size_t Dims, typename Traits>
class StaticKDTree {
 public:
    explicit StaticKDTree(const std::array<T, N>& items) : items_(items) {
        build<0>(0, items.size());
    }

    const T& getClosest(const T& to) const {
        uint32_t min_dist = std::numeric_limits<uint32_t>::max();
        uint32_t index = None;
        getClosestImpl<0>(to, 0, min_dist, index);
        PDFT_VERIFY(index != None);
        return items_[index];
    }

 private:
    static constexpr size_t NumNodes = 2 * N - 1;
    static constexpr size_t None = std::numeric_limits<uint32_t>::max();
    static constexpr size_t Root = 0;

    struct Node {
        uint32_t index;
        std::array<uint32_t, 2> c;
    };

    template <uint32_t D>
    void getClosestImpl(const T& to, uint32_t v, uint32_t& min_dist, uint32_t& index) const {
        if (v == None) {
            return;
        }

        auto&& node = nodes_[v];
        const auto& curr = items_[node.index];
        const auto dist = Traits::distance(curr, to);

        if (dist < min_dist) {
            min_dist = dist;
            index = node.index;
        }

        const auto dir = closestDirection<D>(to, curr);
        const auto next_dim = nextDimension<D>;

        getClosestImpl<next_dim>(to, node.c[dir], min_dist, index);

        if (Traits::template axisDistance<D>(to, curr) < min_dist) {
            getClosestImpl<next_dim>(to, node.c[1 - dir], min_dist, index);
        }
    }

    template <uint32_t D>
    uint32_t build(uint32_t begin, uint32_t end) {
        const uint32_t size = end - begin;

        if (size == 0) {
            return None;
        }

        auto&& [index, node] = allocate();

        if (size == 1) {
            node = {
                .index = begin,
                .c = {None, None},
            };
            return index;
        }

        auto it = items_.begin();
        const uint32_t mid = begin + size / 2;

        std::nth_element(it + begin, it + mid, it + end, [](auto&& p1, auto&& p2) {
            return Traits::template nthDim<D>(p1) < Traits::template nthDim<D>(p2);
        });

        constexpr uint32_t next_dim = nextDimension<D>;

        node = {
            .index = mid,
            .c =
                {
                    build<next_dim>(begin, mid),
                    build<next_dim>(mid, end),
                },
        };
        return index;
    }

    std::tuple<uint32_t, Node&> allocate() {
        PDFT_VERIFY(next_node_ < NumNodes);
        uint32_t index = next_node_++;
        return std::tie(index, nodes_[index]);
    }

    template <uint32_t D>
    static constexpr uint32_t nextDimension = D == Dims - 1 ? 0 : D + 1;

    template <uint32_t D>
    static uint32_t closestDirection(const T& query, const T& current) {
        return Traits::template nthDim<D>(query) <= Traits::template nthDim<D>(current) ? 0 : 1;
    }

    std::array<T, N> items_;
    std::array<Node, NumNodes> nodes_;
    size_t next_node_ = 0;
};  // namespace pdft::utils

}  // namespace pdft::utils
