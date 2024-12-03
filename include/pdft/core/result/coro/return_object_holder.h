#pragma once

#include <utility>

template <typename T>
class ReturnedObjectHolder {
 public:
    explicit ReturnedObjectHolder(ReturnedObjectHolder** owner) : object{}, owner(owner) {
        *owner = this;
    }

    ReturnedObjectHolder(ReturnedObjectHolder const&) = delete;
    ReturnedObjectHolder(ReturnedObjectHolder&& other)
        : object(std::move(other.object))
        , owner(std::exchange(other.owner, nullptr)) {
        *owner = this;
    }

    ReturnedObjectHolder& operator=(ReturnedObjectHolder const&) = delete;
    ReturnedObjectHolder& operator=(ReturnedObjectHolder&&) = delete;

    T& mutObject() {
        return object;
    }

    /* implicit */ operator T() {
        return std::move(object);
    }

 private:
    T object;
    ReturnedObjectHolder** owner = nullptr;
};
