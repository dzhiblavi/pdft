#pragma once

#include "pdft/core/assert.h"

#include <optional>
#include <utility>

template <typename T>
class ReturnedObjectHolder {
 public:
    explicit ReturnedObjectHolder(ReturnedObjectHolder** owner) : owner_(owner) {
        *owner_ = this;
    }

    // Pinned
    ReturnedObjectHolder(ReturnedObjectHolder&&) = delete;
    ReturnedObjectHolder(ReturnedObjectHolder const&) = delete;
    ReturnedObjectHolder& operator=(ReturnedObjectHolder const&) = delete;
    ReturnedObjectHolder& operator=(ReturnedObjectHolder&&) = delete;

    void* ptr() {
        return &object_;
    }

    void assign(T object) {
        PDFT_VERIFY_DBG(!object_.has_value(), "double assign");
        object_.emplace(std::move(object));
    }

    /* implicit */ operator T() && {
        PDFT_VERIFY_DBG(object_.has_value(), "not assigned");
        return std::move(*object_);
    }

 private:
    std::optional<T> object_ = std::nullopt;
    ReturnedObjectHolder** owner_ = nullptr;
};
