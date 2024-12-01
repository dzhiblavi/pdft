#include "pdft/image/palette.h"

namespace pdft::image {

namespace {

template <size_t MaxSize>
struct Cache {
    void insert(const RGBColor& from, int value) {
        const size_t index = find(from);

        if (index != MaxSize) {
            ++cache[index].count;
            return;
        }

        const size_t lowest = find_lowest_count();

        cache[lowest] = {
            .from = from,
            .count = 1,
            .to = value,
        };
    }

    bool get(const RGBColor& from, int& value) {
        const size_t index = find(from);

        if (index == MaxSize) {
            return false;
        }

        value = cache[index].to;
        return true;
    }

    size_t find_lowest_count() {
        size_t index = 0;
        int lowest = std::numeric_limits<int>::max();

        for (size_t i = 0; i < MaxSize; ++i) {
            auto&& entry = cache[i];

            if (entry.to != -1 && entry.count < lowest) {
                index = i;
                lowest = entry.count;
            }
        }

        return index;
    }

    size_t find(const RGBColor& from) {
        for (size_t i = 0; i < MaxSize; ++i) {
            auto&& entry = cache[i];

            if (entry.to != -1 && entry.from == from) {
                return i;
            }
        }

        return MaxSize;
    }

    struct Entry {
        RGBColor from;
        int count = 0;
        int to = -1;
    };

    std::array<Entry, MaxSize> cache;
};

uint32_t square(int32_t value) {
    return value * value;
}

uint32_t getDistance(const RGBColor& p, const RGBColor& clr) {
    int32_t r = p.r;
    int32_t g = p.g;
    int32_t b = p.b;
    return square(r - clr.r) + square(g - clr.g) + square(b - clr.b);
}

int getClosest(const RGBColor& pixel, PaletteRef palette) {
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

const std::array<RGBColor, 2>& getDefaultStaticPalette() {
    // https://colorhunt.co/palette/1e201e3c3d37697565ecdfcc
    static const std::array<RGBColor, 2> palette{
        // rgbFromHex("#1E201E"),
        // rgbFromHex("#3C3D37"),
        // rgbFromHex("#697565"),
        // rgbFromHex("#ECDFCC"),
        rgbFromHex("#2d2d2d"),
        rgbFromHex("#ffffff"),
    };

    return palette;
}

}  // namespace

void applyPalette(std::span<uint8_t> pixels, PaletteRef palette) {
    // Two-element cache for background and text colors
    Cache<2> cache;

    const size_t num_pixels = pixels.size() / 3;

    for (size_t i = 0; i < num_pixels; ++i) {
        RGBColor& pixel = *reinterpret_cast<RGBColor*>(&pixels[3 * i]);

        int index;
        if (!cache.get(pixel, index)) {
            index = getClosest(pixel, palette);
            cache.insert(pixel, index);
        }

        pixel = palette[index];
    }
}

PaletteRef getDefaultPalette() {
    return getDefaultStaticPalette();
}

}  // namespace pdft::image
