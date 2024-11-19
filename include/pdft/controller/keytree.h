#pragma once

#include <functional>
#include <unordered_map>

namespace pdft::controller {

class KeyTree {
 public:
    using CallbackType = std::function<void(uint32_t)>;

    KeyTree() = default;
    ~KeyTree() noexcept;

    struct NumberType {};
    inline static constexpr NumberType number;

    void bind(std::string_view keys, CallbackType callback);
    void bind(NumberType, std::string_view keys, CallbackType callback);

    void feed(char key);
    void reset();

 private:
    struct Node {
        CallbackType callback = {};
        std::unordered_map<char, Node*> children;
    };

    enum State {
        None,
        Simple,
        Counting,
        // Number otherwise
    };

    void feedChar(char key);
    void feedDigit(char key);

    static void bindFrom(Node* root, std::string_view keys, CallbackType callback);

    Node* num_root_ = nullptr;
    Node* root_ = nullptr;
    Node* curr_ = nullptr;
    State state_ = State::None;
    uint32_t number_ = 0;
};

}  // namespace pdft::controller
