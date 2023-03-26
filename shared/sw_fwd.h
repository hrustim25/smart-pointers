#pragma once

#include <array>
#include <exception>

struct ControlBlockBase {
public:
    virtual ~ControlBlockBase() = default;

    virtual void DeleteObject() {
    }

    void IncCounter() {
        ++counter_;
    }
    void DecCounter() {
        --counter_;
        if (counter_ == 0) {
            if (weak_counter_ == 0) {
                delete this;
            } else {
                DeleteObject();
            }
        }
    }
    size_t GetCounter() const {
        return counter_;
    }

    void IncWeakCounter() {
        ++weak_counter_;
    }
    void DecWeakCounter() {
        --weak_counter_;
        if (weak_counter_ == 0) {
            if (counter_ == 0) {
                delete this;
            }
        }
    }
    size_t GetWeakCounter() const {
        return weak_counter_;
    }

private:
    size_t counter_ = 0;
    size_t weak_counter_ = 0;
};

template <typename T>
struct ControlBlockPtr : public ControlBlockBase {
public:
    ControlBlockPtr(T* other_ptr = nullptr) : ptr_(other_ptr) {
        IncCounter();
    }
    ~ControlBlockPtr() override {
        delete ptr_;
    }
    T* GetPointer() const {
        return ptr_;
    }

    void DeleteObject() override {
        delete ptr_;
        ptr_ = nullptr;
    }

private:
    T* ptr_;
};

template <typename T>
struct ControlBlockObject : public ControlBlockBase {
public:
    template <typename... Args>
    ControlBlockObject(Args&&... args) {
        new (&storage_) T(std::forward<Args>(args)...);
        IncCounter();
    }
    ~ControlBlockObject() {
        reinterpret_cast<T*>(&storage_)->~T();
    }
    T* GetPointer() {
        return reinterpret_cast<T*>(&storage_);
    }

    void DeleteObject() override {
        reinterpret_cast<T*>(&storage_)->~T();
        storage_.fill(0);
    }

private:
    alignas(T) std::array<char, sizeof(T)> storage_;
};

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

class ESFTBase {};

template <typename T>
class EnableSharedFromThis;
