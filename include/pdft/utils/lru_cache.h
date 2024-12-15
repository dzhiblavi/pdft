#pragma once

#include <list>
#include <unordered_map>

namespace pdft::utils {

template <typename K, typename V>
class LRUCache {
 public:
    using KVPair = std::pair<K, V>;

    explicit LRUCache(size_t max_size) : max_size_(max_size) {}

    std::optional<KVPair> add(const K& key, V value) {
        if (auto it = nodes_.find(key); it != nodes_.end()) {
            reassign(it->second, std::move(value));
            return std::nullopt;
        }

        std::optional<KVPair> dropped;
        if (size() == max_size_) {
            dropped = dropLru();
        }

        addFront(key, std::move(value));
        return dropped;
    }

    const V* tryGet(const K& key) {
        MapIteratorType it = nodes_.find(key);

        if (it == nodes_.end()) {
            return nullptr;
        }

        ListIteratorType list_it = it->second;
        moveToFront(list_it);
        return &list_it->second;
    }

    bool remove(const K& key) {
        if (auto it = nodes_.find(key); it != nodes_.end()) {
            list_.erase(it->second);
            nodes_.erase(it);
            return true;
        }

        return false;
    }

    void clear() {
        list_.clear();
        nodes_.clear();
    }

    bool contains(const K& key) const {
        return nodes_.contains(key);
    }

    bool empty() const noexcept {
        return size() == 0;
    }

    size_t size() const noexcept {
        return list_.size();
    }

    size_t capacity() const noexcept {
        return max_size_;
    }

 private:
    using ListType = std::list<KVPair>;
    using ListIteratorType = ListType::iterator;
    using MapType = std::unordered_map<K, ListIteratorType>;
    using MapIteratorType = MapType::iterator;

    void reassign(ListIteratorType it, V value) {
        it->second = std::move(value);
        moveToFront(it);
    }

    void moveToFront(ListIteratorType it) {
        list_.splice(list_.begin(), list_, it);
    }

    KVPair dropLru() {
        ListIteratorType last = std::prev(list_.end());
        auto [k, v] = std::move(*last);

        nodes_.erase(k);
        list_.erase(last);
        return std::make_pair(k, std::move(v));
    }

    void addFront(const K& key, V value) {
        ListIteratorType it = list_.emplace(list_.begin(), key, std::move(value));
        nodes_.emplace(key, it);
    }

    const size_t max_size_;
    ListType list_;
    MapType nodes_;
};

}  // namespace pdft::utils
