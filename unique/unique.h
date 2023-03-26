#pragma once

#include "compressed_pair.h"

#include <cstddef>

template <typename T>
struct Slug {
public:
    Slug() = default;

    template <typename Slug2>
    Slug(const Slug2&) {
    }

    void operator()(const T* value) {
        delete value;
    }
};

template <typename T>
struct Slug<T[]> {
public:
    Slug() = default;

    template <typename Slug2>
    Slug(const Slug2&) {
    }

    void operator()(T* value) {
        delete[] value;
    }
};

template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : pair_(ptr, Deleter()) {
    }
    UniquePtr(T* ptr, Deleter deleter) : pair_(ptr, std::move(deleter)) {
    }

    template <typename U, typename Deleter2>
    UniquePtr(UniquePtr<U, Deleter2>&& other) noexcept {
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;

        pair_.GetFirst() = std::move(static_cast<T*>(other.Release()));
        pair_.GetSecond() = std::move(other.GetDeleter());
    }

    UniquePtr(UniquePtr&& other) noexcept {
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;

        pair_ =
            CompressedPair(std::move(other.pair_.GetFirst()), std::move(other.pair_.GetSecond()));
        other.pair_.GetFirst() = nullptr;
    }

    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other == this) {
            return *this;
        }
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;

        pair_ =
            CompressedPair(std::move(other.pair_.GetFirst()), std::move(other.pair_.GetSecond()));
        other.pair_.GetFirst() = nullptr;
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) {
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;
        return *this;
    }

    // Destructor

    ~UniquePtr() {
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;
    }

    // Modifiers

    T* Release() {
        T* result = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return result;
    }
    void Reset(T* ptr = nullptr) {
        T* tmp = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        pair_.GetSecond()(tmp);
        if (ptr != nullptr) {
            pair_.GetFirst() = ptr;
        }
    }
    void Swap(UniquePtr& other) {
        CompressedPair<T*, Deleter> tmp(std::move(pair_));
        pair_ = std::move(other.pair_);
        other.pair_ = std::move(tmp);
    }

    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }
    Deleter& GetDeleter() {
        return pair_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    }
    explicit operator bool() const {
        return pair_.GetFirst() != nullptr;
    }

    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *pair_.GetFirst();
    }
    T* operator->() const {
        return pair_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> pair_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : pair_(ptr, Deleter()) {
    }
    UniquePtr(T* ptr, Deleter deleter) : pair_(ptr, std::move(deleter)) {
    }

    template <typename U, typename Deleter2>
    UniquePtr(UniquePtr<U, Deleter2>&& other) noexcept {
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;

        pair_.GetFirst() = std::move(static_cast<T*>(other.Release()));
        pair_.GetSecond() = std::move(other.GetDeleter());
    }

    UniquePtr(UniquePtr&& other) noexcept {
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;

        pair_ =
            CompressedPair(std::move(other.pair_.GetFirst()), std::move(other.pair_.GetSecond()));
        other.pair_.GetFirst() = nullptr;
    }

    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other == this) {
            return *this;
        }
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;

        pair_ =
            CompressedPair(std::move(other.pair_.GetFirst()), std::move(other.pair_.GetSecond()));
        other.pair_.GetFirst() = nullptr;
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) {
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;
        return *this;
    }

    // Destructor

    ~UniquePtr() {
        pair_.GetSecond()(pair_.GetFirst());
        pair_.GetFirst() = nullptr;
    }

    // Modifiers

    T* Release() {
        T* result = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return result;
    }
    void Reset(T* ptr = nullptr) {
        T* tmp = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        pair_.GetSecond()(tmp);
        if (ptr != nullptr) {
            pair_.GetFirst() = ptr;
        }
    }
    void Swap(UniquePtr& other) {
        CompressedPair<T*, Deleter> tmp(std::move(pair_));
        pair_ = std::move(other.pair_);
        other.pair_ = std::move(tmp);
    }

    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }
    Deleter& GetDeleter() {
        return pair_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    }
    explicit operator bool() const {
        return pair_.GetFirst() != nullptr;
    }

    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *pair_.GetFirst();
    }
    T* operator->() const {
        return pair_.GetFirst();
    }
    std::add_lvalue_reference_t<T> operator[](size_t index) const {
        return pair_.GetFirst()[index];
    }

private:
    CompressedPair<T*, Deleter> pair_;
};
