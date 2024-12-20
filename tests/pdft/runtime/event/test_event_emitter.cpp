#include "pdft/runtime/event/event_emitter.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::rt {

namespace {

template <size_t Base, size_t N>
bool nextBase(std::array<int, N>& x) {
    size_t i = N - 1;
    while (i > 0 && x[i] == Base - 1) {
        --i;
    }
    if (x[i] == Base - 1) {
        return false;
    }
    ++x[i];
    ++i;
    for (; i < N; ++i) {
        x[i] = 0;
    }
    return true;
};

}  // namespace

using Emitter = EventEmitter<int>;

struct Slot : SlotBase<int> {
    Slot* unsub = nullptr;
    int last = 0;

    void consume(const Event<int>& event) override {
        last = event.value;

        if (unsub) {
            // inflight unsubscribe
            unsub->unplug();
        }
    }
};

TEST_CASE("EventEmitter/no-subscribers") {
    Emitter e;
    e.emit(1);
}

TEST_CASE("EventEmitter/one-subscriber/no-unsubscribe") {
    Emitter e;
    Slot s;
    s.plug(e);

    e.emit(1);
    CHECK(s.last == 1);

    e.emit(2);
    CHECK(s.last == 2);
}

TEST_CASE("EventEmitter/one-subscriber/unsubscribe-self-between") {
    Emitter e;
    Slot s;
    s.plug(e);

    e.emit(1);
    CHECK(s.last == 1);

    s.unplug();

    e.emit(2);
    CHECK(s.last == 1);
}

TEST_CASE("EventEmitter/one-subscriber/unsubscribe-self-inflight") {
    Emitter e;
    Slot s;
    s.unsub = &s;
    s.plug(e);

    e.emit(1);
    CHECK(s.last == 1);

    e.emit(2);
    CHECK(s.last == 1);
}

TEST_CASE("EventEmitter/multiple-subscribers/no-unsubscribe") {
    constexpr size_t n = 5;
    Emitter e;
    std::array<Slot, n> slots;

    for (auto& s : slots) {
        s.plug(e);
    }

    e.emit(1);
    for (auto& s : slots) {
        CHECK(s.last == 1);
    }

    e.emit(2);
    for (auto& s : slots) {
        CHECK(s.last == 2);
    }
}

TEST_CASE("EventEmitter/multiple-subscribers/unsubscribe-self-between") {
    constexpr size_t n = 5;
    std::array<int, n> unsubs{0};

    int value = 1;
    do {  // NOLINT
        Emitter e;
        std::array<Slot, n> slots;
        for (size_t i = 0; i < n; ++i) {
            slots[i].plug(e);
        }

        e.emit(value);
        for (auto& s : slots) {
            CHECK(s.last == value);
        }

        for (size_t i = 0; i < n; ++i) {
            if (unsubs[i]) {
                slots[i].unplug();
            }
        }

        ++value;
        e.emit(value);

        for (size_t i = 0; i < n; ++i) {
            if (unsubs[i]) {
                CHECK(slots[i].last == value - 1);
            } else {
                CHECK(slots[i].last == value);
            }
        }
    } while (nextBase<2>(unsubs));
}

TEST_CASE("EventEmitter/multiple-subscribers/unsubscribe-self-inflight") {
    constexpr size_t n = 5;
    std::array<int, n> unsubs{0};

    int value = 1;
    do {  // NOLINT
        Emitter e;
        std::array<Slot, n> slots;
        for (size_t i = 0; i < n; ++i) {
            if (unsubs[i]) {
                slots[i].unsub = &slots[i];
            }
            slots[i].plug(e);
        }

        e.emit(value);
        for (auto& s : slots) {
            CHECK(s.last == value);
        }

        ++value;
        e.emit(value);

        for (size_t i = 0; i < n; ++i) {
            if (unsubs[i]) {
                CHECK(slots[i].last == value - 1);
            } else {
                CHECK(slots[i].last == value);
            }
        }
    } while (nextBase<2>(unsubs));
}

TEST_CASE("EventEmitter/multiple-subscribers/unsubscribe-self-mixed") {
    constexpr size_t n = 5;

    // 0 = do not unsubscribe
    // 1 = unsubscribe between
    // 2 = unsubscribe inflight
    std::array<int, n> unsubs{0};

    int value = 1;
    do {  // NOLINT
        Emitter e;
        std::array<Slot, n> slots;

        for (size_t i = 0; i < n; ++i) {
            if (unsubs[i] == 2) {
                // unsubscribe inflight
                slots[i].unsub = &slots[i];
            }
            slots[i].plug(e);
        }

        e.emit(value);
        for (auto& s : slots) {
            CHECK(s.last == value);
        }

        for (size_t i = 0; i < n; ++i) {
            if (unsubs[i] == 1) {
                // unsubscribe between
                slots[i].unplug();
            }
        }

        ++value;
        e.emit(value);

        for (size_t i = 0; i < n; ++i) {
            if (unsubs[i] != 0) {
                // unsubscribed anyhow
                CHECK(slots[i].last == value - 1);
            } else {
                CHECK(slots[i].last == value);
            }
        }
    } while (nextBase<3>(unsubs));
}

TEST_CASE("EventEmitter/multiple-subscribers/unsubscribe-anyone-mixed") {
    constexpr int n = 3;

    // 0 = do not unsubscribe
    // 2k = unsubscribe kth between
    // 2k + 1 = unsubscribe kth inflight
    std::array<int, n> unsubs{0};

    auto unsub = [&](int i) { return unsubs[i] != 0; };
    auto inflight = [&](int i) {
        auto x = unsubs[i];
        return x > 0 && (x % 2) == 1;
    };
    auto index = [&](int i) {
        auto x = unsubs[i];
        return (x - 1) / 2;
    };
    auto unsubbed = [&](int i) {
        for (int j = 0; j < n; ++j) {
            if (unsub(j) && index(j) == i) {
                return true;
            }
        }
        return false;
    };

    int value = 1;
    do {  // NOLINT
        Emitter e;
        std::array<Slot, n> slots;

        for (int i = 0; i < n; ++i) {
            slots[i].plug(e);

            if (unsub(i) && inflight(i)) {
                slots[i].unsub = &slots[index(i)];
            }
        }

        e.emit(value);
        for (auto& s : slots) {
            CHECK(s.last == value);
        }

        for (int i = 0; i < n; ++i) {
            if (unsub(i) && !inflight(i)) {
                slots[index(i)].unplug();
            }
        }

        ++value;
        e.emit(value);

        for (int i = 0; i < n; ++i) {
            if (unsubbed(i)) {
                // unsubscribed anyhow
                CHECK(slots[i].last == value - 1);
            } else {
                CHECK(slots[i].last == value);
            }
        }
    } while (nextBase<2 * n + 1>(unsubs));  // 2^9 combinations
}

}  // namespace pdft::rt
