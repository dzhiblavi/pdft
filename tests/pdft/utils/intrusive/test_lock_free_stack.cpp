#include "pdft/core/assert.h"
#include "pdft/utils/intrusive/mpmc_stack.h"
#include "pdft/utils/intrusive/node.h"
#include "pdft/utils/random.h"

#include <catch2/catch_test_macros.hpp>

#include <thread>

namespace pdft::utils::intrusive {

struct Item : ForwardListItem<Item> {
    int value = 0;
};

using List = MPMCStack<Item>;

TEST_CASE("intrusive/MPMCStack/empty", "") {
    List list;

    int count = 0;
    list.forEach([&](auto&&) { ++count; });
    CHECK(count == 0);
}

TEST_CASE("intrusive/MPMCStack/single", "") {
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

TEST_CASE("intrusive/MPMCStack/popFront", "") {
    List list;
    Item item{.value = 10};
    list.push(item);

    CHECK(list.tryPop()->value == 10);
}

TEST_CASE("intrusive/MPMCStack/popFront/push-same", "") {
    List list;
    Item item{.value = 10};
    list.push(item);

    CHECK(list.tryPop()->value == 10);

    list.push(item);
    CHECK(list.tryPop()->value == 10);
}

TEST_CASE("intrusive/MPMCStack/push-many", "") {
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

TEST_CASE("intrusive/MPMCStack/move-between-lists", "") {
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

TEST_CASE("intrusive/MPMCStack/stress/mpmc") {
    std::vector<Item> items{8128};
    std::vector<List> lists{32};

    int value = 0;
    for (auto& item : items) {
        item.value = value++;
        auto idx = utils::random<uint32_t>(0, lists.size() - 1);
        lists[idx].push(item);
    }

    auto worker = [&] {
        int iterations = 65536;

        while (iterations--) {
            auto from = utils::random<uint32_t>(0, lists.size() - 1);
            auto to = utils::random<uint32_t>(0, lists.size() - 1);

            if (auto* item = lists[from].tryPop()) {
                lists[to].push(*item);
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    // The result will not always be correct because
    // of the ABA problem.
}

}  // namespace pdft::utils::intrusive
