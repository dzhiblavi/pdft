#include "pdft/image/palette.h"

namespace pdft::image {

namespace {

template <size_t MaxSize>
struct Cache {
    void insert(const IRGBColor& from, size_t value) {
        const size_t index = find(from);

        if (index != MaxSize) {
            ++cache[index].count;
            return;
        }

        const size_t lowest = findLowestCount();

        cache[lowest] = {
            .from = from,
            .to = value,
            .count = 1,
        };
    }

    bool get(const IRGBColor& from, size_t& value) {
        const size_t index = find(from);

        if (index == MaxSize) {
            return false;
        }

        value = cache[index].to;
        return true;
    }

    size_t findLowestCount() {
        size_t index = 0;
        int lowest = std::numeric_limits<int>::max();

        for (size_t i = 0; i < MaxSize; ++i) {
            auto&& entry = cache[i];

            if (entry.to != None && entry.count < lowest) {
                index = i;
                lowest = entry.count;
            }
        }

        return index;
    }

    size_t find(const IRGBColor& from) {
        for (size_t i = 0; i < MaxSize; ++i) {
            auto&& entry = cache[i];

            if (entry.to != None && entry.from == from) {
                return i;
            }
        }

        return MaxSize;
    }

    static constexpr size_t None = static_cast<size_t>(-1);

    struct Entry {
        IRGBColor from{};
        size_t to = None;
        int count = 0;
    };

    std::array<Entry, MaxSize> cache;
};

uint32_t square(int32_t value) {
    return value * value;
}

uint32_t getDistance(const IRGBColor& p, const IRGBColor& clr) {
    int32_t r = p.r;
    int32_t g = p.g;
    int32_t b = p.b;
    return square(r - clr.r) + square(g - clr.g) + square(b - clr.b);
}

size_t getClosest(const IRGBColor& pixel, PaletteRef palette) {
    size_t index = 0;
    uint32_t distance = getDistance(pixel, palette[0]);

    for (size_t i = 1; i < palette.size(); ++i) {
        uint32_t dist = getDistance(pixel, palette[i]);

        if (dist < distance) {
            distance = dist;
            index = i;
        }
    }

    return index;
}

}  // namespace

void applyPalette(std::span<IRGBColor> pixels, PaletteRef palette) {
    // Two-element cache for background and text colors
    Cache<2> cache;

    for (auto& pixel : pixels) {
        size_t index = 0;

        if (!cache.get(pixel, index)) {
            index = getClosest(pixel, palette);
            cache.insert(pixel, index);
        }

        pixel = palette[index];
    }
}

}  // namespace pdft::image
