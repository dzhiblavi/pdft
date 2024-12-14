#include "pdft/core/trace/log_stack_trace.h"

namespace pdft::trace::detail {

std::string getAutomaticPrettyDuration(const DurationType& duration) {
    static constexpr std::array<uint64_t, 4> ratios{
        1000000000,  // seconds
        1000000,     // milliseconds
        1000,        // microseconds
        1,           // nanoseconds
    };
    static constexpr std::array<std::string_view, 4> names{
        "s",
        "ms",
        "us",
        "ns",
    };

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

    size_t index = 0;
    while (index + 1 < ratios.size() && ns / ratios[index] == 0) {
        ++index;
    }

    return std::format(
        "{:.2f}{}", static_cast<float>(ns) / static_cast<float>(ratios[index]), names[index]);
}

}  // namespace pdft::trace::detail
