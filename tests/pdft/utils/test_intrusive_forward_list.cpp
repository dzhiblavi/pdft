#include "pdft/utils/intrusive_forward_list.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::utils {

struct Item : IntrusiveForwardListItem<Item> {
    int value = 0;
};

using List = IntrusiveForwardList<Item>;

TEST_CASE("testIntrusiveForwardListEmpty", "") {
    List list;

    CHECK(list.size() == 0);
    CHECK(list.isEmpty());

    int count = 0;
    list.forEach([&](auto&&) { ++count; });
    CHECK(count == 0);
}

TEST_CASE("testIntrusiveForwardListSingle", "") {
    List list;
    Item item{.value = 10};
    list.pushFront(item);

    CHECK(list.size() == 1);
    CHECK(!list.isEmpty());
    int count = 0;
    list.forEach([&](auto& value) {
        ++count;
        CHECK(value.value == 10);
    });
    CHECK(count == 1);

    CHECK(list.front().value == 10);
}

TEST_CASE("testIntrusiveForwardListPopFront", "") {
    List list;
    Item item{.value = 10};
    list.pushFront(item);

    CHECK(list.popFront().value == 10);
    CHECK(list.size() == 0);
}

TEST_CASE("testIntrusiveForwardListPopFrontPushSame", "") {
    List list;
    Item item{.value = 10};
    list.pushFront(item);

    CHECK(list.popFront().value == 10);

    list.pushFront(item);
    CHECK(list.size() == 1);
    CHECK(list.front().value == 10);
}

TEST_CASE("testIntrusiveForwardListPushMany", "") {
    List list;
    std::vector<Item> items;
    items.reserve(10);

    for (int i = 0; i < 10; ++i) {
        items.push_back(Item{.value = i});
        list.pushFront(items.back());
    }

    CHECK(list.size() == 10);
    CHECK(!list.isEmpty());

    int count = 0;
    list.forEach([&](auto& value) { CHECK(value.value == count++); });
    CHECK(count == 10);
}

TEST_CASE("testIntrusiveForwardListMoveBetweenLists", "") {
    List list1, list2;
    std::vector<Item> items;
    items.reserve(10);

    for (int i = 0; i < 10; ++i) {
        items.push_back(Item{.value = i});

        if (i % 2 == 0) {
            list1.pushFront(items.back());
        } else {
            list2.pushFront(items.back());
        }
    }

    CHECK(list1.size() == 5);
    CHECK(list2.size() == 5);

    while (!list1.isEmpty()) {
        list2.pushFront(list1.popFront());
    }

    CHECK(list1.size() == 0);
    CHECK(list2.size() == 10);

    while (!list2.isEmpty()) {
        list1.pushFront(list2.popFront());
    }

    CHECK(list2.size() == 0);
    CHECK(list1.size() == 10);
}

}  // namespace pdft::utils
