#include "pdft/tui/kitty/apc/parse_response.h"

namespace pdft::tui {

namespace {

using ControlDataItem = std::pair<std::string, std::string>;
using ControlData = std::vector<ControlDataItem>;

Result<ControlDataItem> parseControlDataItem(std::string_view item_view) {
    auto equal_sign_pos = item_view.find('=');

    if (equal_sign_pos == std::string::npos || equal_sign_pos == 0) {
        return result::err("malformed response");
    }

    return std::make_pair(
        std::string(item_view.substr(0, equal_sign_pos)),
        std::string(item_view.substr(equal_sign_pos + 1)));
}

Result<ControlData> parseControlData(std::string_view control_data) {
    ControlData data;
    size_t pos = 0;

    while (pos < control_data.size()) {
        size_t kv_end = control_data.find(',', pos);
        std::string_view kv_view;

        if (kv_end == std::string::npos) {
            kv_view = control_data.substr(pos);
            kv_end = control_data.size();
        } else {
            kv_view = control_data.substr(pos, kv_end - pos);
        }

        if (auto item = parseControlDataItem(kv_view); item.has_value()) {
            data.push_back(std::move(item.value()));
        } else {
            return std::unexpected(item.error());
        }

        pos = kv_end + 1;
    }

    return data;
}

}  // namespace

// \033_Ga=b,c=d;OK\033 and backslash
Result<Response> parseResponse(std::string_view response) {
    if (response.size() <= 4) {
        return result::err("malformed response");
    }

    if (response[0] != '\033') {
        return result::err("malformed response");
    }

    if (response.back() != '\\') {
        return result::err("malformed response");
    }

    // The first two bytes after escape code are interpreted as command code (_G)
    auto command_view = response.substr(1, 2);

    // Search for ; starting from index 3
    auto end_of_control_data = response.find(';', 3);

    if (end_of_control_data == std::string::npos) {
        return result::err("malformed response");
    }

    auto control_data_view = response.substr(3, end_of_control_data - 3);
    auto payload_view =
        response.substr(end_of_control_data + 1, response.size() - end_of_control_data - 3);

    return parseControlData(control_data_view).transform([&](auto control_data) {
        return Response{
            .command = std::string(command_view),
            .payload = std::string(payload_view),
            .control_data = std::move(control_data),
        };
    });
}

}  // namespace pdft::tui
