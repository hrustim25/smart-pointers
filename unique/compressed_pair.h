#pragma once

#include <type_traits>
#include <utility>

template <class V>
inline constexpr bool is_comp_v = std::is_empty_v<V> && !std::is_final_v<V>;

// Me think, why waste time write lot code, when few code do trick.
template <typename F, typename S, bool e1 = is_comp_v<F>, bool e2 = is_comp_v<S>>
class CompressedPair {
public:
    CompressedPair() : first_(F()), second_(S()) {
    }

    CompressedPair(const F &first, const S &second) : first_(first), second_(second) {
    }
    CompressedPair(F &&first, const S &second) : first_(std::move(first)), second_(second) {
    }
    CompressedPair(const F &first, S &&second) : first_(first), second_(std::move(second)) {
    }
    CompressedPair(F &&first, S &&second) : first_(std::move(first)), second_(std::move(second)) {
    }

    CompressedPair(const CompressedPair &other) : first_(other.first_), second_(other.second_) {
    }
    CompressedPair(CompressedPair &&other)
        : first_(std::move(other.first_)), second_(std::move(other.second_)) {
    }

    CompressedPair &operator=(CompressedPair &&other) {
        first_ = std::move(other.first_);
        second_ = std::move(other.second_);
        return *this;
    }

    F &GetFirst() {
        return first_;
    }
    const F &GetFirst() const {
        return first_;
    }

    S &GetSecond() {
        return second_;
    }
    const S &GetSecond() const {
        return second_;
    };

private:
    F first_;
    S second_;
};

template <typename F, typename S>
class CompressedPair<F, S, true, false> : public F {
public:
    CompressedPair() : second_(S()) {
    }

    CompressedPair(const F &, const S &second) : second_(second) {
    }
    CompressedPair(const F &, S &&second) : second_(std::move(second)) {
    }

    CompressedPair(const CompressedPair &other) : second_(other.second_) {
    }
    CompressedPair(CompressedPair &&other) : second_(std::move(other.second_)) {
    }

    CompressedPair &operator=(CompressedPair &&other) {
        second_ = std::move(other.second_);
        return *this;
    }

    F &GetFirst() {
        return *this;
    }
    const F &GetFirst() const {
        return *this;
    }

    S &GetSecond() {
        return second_;
    }
    const S &GetSecond() const {
        return second_;
    };

private:
    S second_;
};

template <typename F, typename S>
class CompressedPair<F, S, false, true> : public S {
public:
    CompressedPair() : first_(F()) {
    }

    CompressedPair(const F &first, const S &) : first_(first) {
    }
    CompressedPair(F &&first, const S &) : first_(std::move(first)) {
    }

    CompressedPair(const CompressedPair &other) : first_(other.first_) {
    }
    CompressedPair(CompressedPair &&other) : first_(std::move(other.first_)) {
    }

    CompressedPair &operator=(CompressedPair &&other) {
        first_ = std::move(other.first_);
        return *this;
    }

    F &GetFirst() {
        return first_;
    }
    const F &GetFirst() const {
        return first_;
    }

    S &GetSecond() {
        return *this;
    }
    const S &GetSecond() const {
        return *this;
    };

private:
    F first_;
};

template <typename F, typename S>
class CompressedPair<F, S, true, true> : public F, public S {
public:
    CompressedPair() {
    }

    CompressedPair(const F &, const S &) {
    }

    CompressedPair(const CompressedPair &) {
    }
    CompressedPair(CompressedPair &&) {
    }

    CompressedPair &operator=(CompressedPair &&) {
        return *this;
    }

    F &GetFirst() {
        return *this;
    }
    const F &GetFirst() const {
        return *this;
    }

    S &GetSecond() {
        return *this;
    }
    const S &GetSecond() const {
        return *this;
    };
};

template <typename F>
class CompressedPair<F, F, true, true> : public F {
public:
    CompressedPair() : first_(F()) {
    }

    CompressedPair(const F &, const F &) {
    }

    CompressedPair(const CompressedPair &) {
    }
    CompressedPair(CompressedPair &&) {
    }

    CompressedPair &operator=(CompressedPair &&other) {
        first_ = std::move(other.first_);
        return *this;
    }

    F &GetFirst() {
        return first_;
    }
    const F &GetFirst() const {
        return first_;
    }

    F &GetSecond() {
        return *this;
    }
    const F &GetSecond() const {
        return *this;
    };

private:
    F first_;
};
