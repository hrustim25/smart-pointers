#pragma once

#include "sw_fwd.h"

#include <cstddef>

template <typename T>
class SharedPtr {
    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class WeakPtr;

public:
    // Constructors

    SharedPtr() : block_(nullptr), ptr_(nullptr) {
    }
    SharedPtr(std::nullptr_t) : block_(nullptr), ptr_(nullptr) {
    }
    explicit SharedPtr(T* ptr) : block_(new ControlBlockPtr(ptr)), ptr_(ptr) {
        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            InitWeakThis(ptr_);
        }
    }
    template <typename Y>
    SharedPtr(Y* ptr) : block_(new ControlBlockPtr(ptr)), ptr_(static_cast<T*>(ptr)) {
        if constexpr (std::is_convertible_v<Y*, ESFTBase*>) {
            InitWeakThis(ptr_);
        }
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) : block_(other.block_), ptr_(static_cast<T*>(other.ptr_)) {
        if (block_) {
            block_->IncCounter();
        }
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) : block_(other.block_), ptr_(static_cast<T*>(other.ptr_)) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    SharedPtr(const SharedPtr& other) {
        block_ = other.block_;
        if (block_) {
            block_->IncCounter();
        }
        ptr_ = other.ptr_;
    }
    SharedPtr(SharedPtr&& other) {
        block_ = other.block_;
        other.block_ = nullptr;

        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : block_(other.block_) {  // ?
        if (other.block_) {
            other.block_->IncCounter();
        }
        ptr_ = ptr;
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {  // to check
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        block_ = other.block_;
        ptr_ = other.ptr_;
        if (block_) {
            block_->IncCounter();
        }
    }

    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }
        if (block_) {
            block_->DecCounter();
        }
        block_ = other.block_;
        if (block_) {
            block_->IncCounter();
        }

        ptr_ = other.ptr_;
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }
        if (block_) {
            block_->DecCounter();
        }
        block_ = other.block_;
        other.block_ = nullptr;

        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    }

    // Destructor

    ~SharedPtr() {
        Reset();
    }

    // Modifiers

    void Reset() {
        if (block_) {
            if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
                if (block_->GetCounter() == 1 && block_->GetWeakCounter() == 1) {
                    ptr_->weak_this_.Reset();
                }
            }
            block_->DecCounter();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    }
    void Reset(T* ptr) {
        if (block_) {
            block_->DecCounter();
        }
        block_ = new ControlBlockPtr(ptr);
        ptr_ = ptr;
    }
    template <typename Y>
    void Reset(Y* ptr) {
        if (block_) {
            block_->DecCounter();
        }
        block_ = new ControlBlockPtr(ptr);
        ptr_ = ptr;
    }

    void Swap(SharedPtr& other) {
        ControlBlockBase* tmp = block_;
        block_ = other.block_;
        other.block_ = tmp;

        std::swap(ptr_, other.ptr_);
    }

    // Observers

    T* Get() const {
        return ptr_;
    }
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    size_t UseCount() const {
        if (block_) {
            return block_->GetCounter();
        }
        return 0;
    }
    explicit operator bool() const {
        return UseCount() > 0;
    }

private:
    SharedPtr(ControlBlockObject<T>* block) : block_(block), ptr_(block->GetPointer()) {
        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            InitWeakThis(ptr_);
        }
    }

    template <typename Y, typename... Args>
    friend SharedPtr<Y> MakeShared(Args&&... args);

    template <typename Y>
    void InitWeakThis(EnableSharedFromThis<Y>* e) {
        e->weak_this_ = *this;
    }

    T* ptr_;
    ControlBlockBase* block_;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
}

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr<T>(new ControlBlockObject<T>(std::forward<Args>(args)...));
}

template <typename T>
class EnableSharedFromThis : public ESFTBase {
    template <typename Y>
    friend class SharedPtr;

public:
    SharedPtr<T> SharedFromThis() {
        return SharedPtr<T>(this->weak_this_);
    }
    SharedPtr<const T> SharedFromThis() const {
        return SharedPtr<const T>(this->weak_this_);
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return WeakPtr<T>(this->weak_this_);
    }
    WeakPtr<const T> WeakFromThis() const noexcept {
        return WeakPtr<const T>(this->weak_this_);
    }

private:
    WeakPtr<T> weak_this_;
};
