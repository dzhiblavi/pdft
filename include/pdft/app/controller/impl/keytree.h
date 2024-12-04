#pragma once

#include <functional>
#include <unordered_map>

namespace pdft::app {

class KeyTree {
 public:
    enum class [[nodiscard]] Result {
        Accepted,    // Key has been appended to the current sequence.
        Rejected,    // Key has been rejected. The state is not affected.
        Terminated,  // Key has been appended and the callback has been called. The state is reset.
    };

    using CallbackType = std::function<void()>;
    using NumberCallbackType = std::function<void(uint32_t)>;

    KeyTree() = default;
    ~KeyTree() noexcept;

    struct NumberType {};
    inline static constexpr NumberType number;

    void bind(std::string_view keys, CallbackType callback);
    void bind(NumberType, std::string_view keys, NumberCallbackType callback);

    Result feed(char key);
    void reset();

 private:
    struct Node {
        std::unordered_map<char, Node*> children;
        [[no_unique_address]] NumberCallbackType callback = {};
    };

    enum State {
        None,
        Simple,
        Counting,
        // Number otherwise
    };

    Result feedChar(char key);
    Result feedDigit(char key);

    static void bindFrom(Node* root, std::string_view keys, NumberCallbackType callback);

    Node* num_root_ = nullptr;
    Node* root_ = nullptr;
    Node* curr_ = nullptr;
    State state_ = State::None;
    uint32_t number_ = 0;
};

}  // namespace pdft::app
