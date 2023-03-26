#pragma once

#include "sw_fwd.h"

template <typename T>
class WeakPtr {
    template <typename Y>
    friend class WeakPtr;

    template <typename Y>
    friend class SharedPtr;

public:
    // Constructors

    WeakPtr() : block_(nullptr), ptr_(nullptr) {
    }

    WeakPtr(const WeakPtr& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncWeakCounter();
        }
    }
    WeakPtr(WeakPtr&& other) : block_(other.block_), ptr_(other.ptr_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template <typename Y>
    WeakPtr(const WeakPtr<Y>& other) : block_(other.block_), ptr_(static_cast<T*>(other.ptr_)) {
        if (block_) {
            block_->IncWeakCounter();
        }
    }
    template <typename Y>
    WeakPtr(WeakPtr<Y>&& other) : block_(other.block_), ptr_(static_cast<T*>(other.ptr_)) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    template <typename Y>
    WeakPtr(const SharedPtr<Y>& other) : block_(other.block_), ptr_(static_cast<T*>(other.ptr_)) {
        if (block_) {
            block_->IncWeakCounter();
        }
    }

    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (&other == this) {
            return *this;
        }
        if (block_) {
            block_->DecWeakCounter();
        }
        block_ = other.block_;
        ptr_ = other.ptr_;
        if (block_) {
            block_->IncWeakCounter();
        }
        return *this;
    }
    WeakPtr& operator=(WeakPtr&& other) {
        if (&other == this) {
            return *this;
        }
        if (block_) {
            block_->DecWeakCounter();
        }
        block_ = other.block_;
        ptr_ = other.ptr_;

        other.block_ = nullptr;
        other.ptr_ = nullptr;
        return *this;
    }

    // Destructor

    ~WeakPtr() {
        if (block_) {
            block_->DecWeakCounter();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    }

    // Modifiers

    void Reset() {
        if (block_) {
            block_->DecWeakCounter();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    }
    void Swap(WeakPtr& other) {
        ControlBlockBase* tmp = block_;
        block_ = other.block_;
        other.block_ = tmp;
    }

    // Observers

    size_t UseCount() const {
        if (block_) {
            return block_->GetCounter();
        }
        return 0;
    }
    bool Expired() const {
        return UseCount() == 0;
    }
    SharedPtr<T> Lock() const {
        return Expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    }

private:
    ControlBlockBase* block_;
    T* ptr_;
};
