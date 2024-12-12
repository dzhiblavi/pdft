#include "pdft/core/assert.h"
#include "pdft/utils/lock_free_stack.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::utils {

struct Item : IntrusiveForwardListItem<Item> {
    int value = 0;
};

using List = MPMCLockFreeStack<Item>;

TEST_CASE("testIntrusiveForwardListEmpty", "") {
    List list;

    int count = 0;
    list.forEach([&](auto&&) { ++count; });
    CHECK(count == 0);
}

TEST_CASE("testIntrusiveForwardListSingle", "") {
    List list;
    Item item{.value = 10};
    list.push(item);

    int count = 0;
    list.forEach([&](auto& value) {
        ++count;
        CHECK(value.value == 10);
    });
    CHECK(count == 1);
}

TEST_CASE("testIntrusiveForwardListPopFront", "") {
    List list;
    Item item{.value = 10};
    list.push(item);

    CHECK(list.tryPop()->value == 10);
}

TEST_CASE("testIntrusiveForwardListPopFrontPushSame", "") {
    List list;
    Item item{.value = 10};
    list.push(item);

    CHECK(list.tryPop()->value == 10);

    list.push(item);
    CHECK(list.tryPop()->value == 10);
}

TEST_CASE("testIntrusiveForwardListPushMany", "") {
    List list;
    std::vector<Item> items;
    items.reserve(10);

    for (int i = 0; i < 10; ++i) {
        items.push_back(Item{.value = i});
        list.push(items.back());
    }

    int count = 0;
    list.forEach([&](auto& value) { CHECK(value.value == 10 - ++count); });
    CHECK(count == 10);
}

TEST_CASE("testIntrusiveForwardListMoveBetweenLists", "") {
    List list1, list2;
    std::vector<Item> items;
    items.reserve(10);

    for (int i = 0; i < 10; ++i) {
        items.push_back(Item{.value = i});

        if (i % 2 == 0) {
            list1.push(items.back());
        } else {
            list2.push(items.back());
        }
    }

    for (int i = 0; i < 5; ++i) {
        list2.push(*PDFT_VERIFY(list1.tryPop()));
    }

    for (int i = 0; i < 5; ++i) {
        list1.push(*PDFT_VERIFY(list2.tryPop()));
    }
}

}  // namespace pdft::utils
