#include "pdft/utils/lru_cache.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::utils {

using Cache = LRUCache<int, int>;

TEST_CASE("LruCache/empty") {
    Cache c(1);
    CHECK(c.empty());
    CHECK(c.size() == 0);
    CHECK(c.capacity() == 1);
}

TEST_CASE("LruCache/empty/try-get") {
    Cache c(1);
    CHECK(c.tryGet(1) == nullptr);
}

TEST_CASE("LruCache/single") {
    Cache c(1);
    CHECK(std::nullopt == c.add(1, 2));
    CHECK(c.size() == 1);
    CHECK(*c.tryGet(1) == 2);
}

TEST_CASE("LruCache/replace-one") {
    Cache c(1);
    CHECK(std::nullopt == c.add(1, 2));
    CHECK(std::make_pair(1, 2) == c.add(2, 3));
    CHECK(c.size() == 1);
    CHECK(c.tryGet(1) == nullptr);
    CHECK(*c.tryGet(2) == 3);
}

TEST_CASE("LruCache/overwrite-one") {
    Cache c(1);
    CHECK(std::nullopt == c.add(1, 2));
    CHECK(std::nullopt == c.add(1, 3));
    CHECK(c.size() == 1);
    CHECK(*c.tryGet(1) == 3);
}

TEST_CASE("LruCache/replace-one-of-three") {
    Cache c(2);
    CHECK(std::nullopt == c.add(1, 1));
    CHECK(std::nullopt == c.add(2, 2));
    CHECK(std::make_pair(1, 1) == c.add(3, 3));
    CHECK(c.size() == 2);
    CHECK(c.tryGet(1) == nullptr);
    CHECK(*c.tryGet(2) == 2);
    CHECK(*c.tryGet(3) == 3);
}

TEST_CASE("LruCache/replace-one-of-three-mtf") {
    Cache c(2);
    CHECK(std::nullopt == c.add(1, 1));
    CHECK(std::nullopt == c.add(2, 2));
    CHECK(std::nullopt == c.add(1, 4));
    CHECK(std::make_pair(2, 2) == c.add(3, 3));
    CHECK(c.size() == 2);
    CHECK(*c.tryGet(1) == 4);
    CHECK(c.tryGet(2) == nullptr);
    CHECK(*c.tryGet(3) == 3);
}

TEST_CASE("LruCache/remove") {
    Cache c(2);
    c.add(1, 1);
    c.add(2, 2);
    CHECK(c.remove(2));
    CHECK(c.size() == 1);
    CHECK(nullptr == c.tryGet(2));
    CHECK(1 == *c.tryGet(1));
}

TEST_CASE("LruCache/remove/non-existent") {
    Cache c(2);
    c.add(1, 1);
    c.add(2, 2);
    CHECK(!c.remove(3));
    CHECK(c.size() == 2);
    CHECK(2 == *c.tryGet(2));
    CHECK(1 == *c.tryGet(1));
}

}  // namespace pdft::utils
