#include "pdft/utils/intrusive/list.h"
#include "pdft/utils/intrusive/node.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::utils::intrusive {

struct Item : ListNode<int> {
    int value = 0;
};

using L = List<Item, int>;

TEST_CASE("intrusive/List/empty", "") {
    L list;
    CHECK(list.empty());
}

TEST_CASE("intrusive/List/push_back", "") {
    L list;
    Item a, b, c;

    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    CHECK(!list.empty());

    int count = 0;
    for (auto& item : list) {
        item.value = ++count;
    }

    CHECK(a.value == 1);
    CHECK(b.value == 2);
    CHECK(c.value == 3);
}

TEST_CASE("intrusive/List/push_front", "") {
    L list;
    Item a, b, c;

    list.push_front(a);
    list.push_front(b);
    list.push_front(c);
    CHECK(!list.empty());

    int count = 0;
    for (auto& item : list) {
        item.value = ++count;
    }

    CHECK(a.value == 3);
    CHECK(b.value == 2);
    CHECK(c.value == 1);
}

TEST_CASE("intrusive/List/pop_back", "") {
    L list;
    Item a, b, c;

    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    list.pop_back();

    int count = 0;
    for (auto& item : list) {
        item.value = ++count;
    }

    CHECK(a.value == 1);
    CHECK(b.value == 2);
    CHECK(c.value == 0);
}

TEST_CASE("intrusive/List/pop_front", "") {
    L list;
    Item a, b, c;

    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    list.pop_front();

    int count = 0;
    for (auto& item : list) {
        item.value = ++count;
    }

    CHECK(a.value == 0);
    CHECK(b.value == 1);
    CHECK(c.value == 2);
}

TEST_CASE("intrusive/List/iterator/empty", "") {
    L list;

    for (Item item : list) {
        std::ignore = item;
        FAIL();
    }
}

}  // namespace pdft::utils::intrusive
