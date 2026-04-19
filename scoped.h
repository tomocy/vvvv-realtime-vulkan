#pragma once

#include <concepts>
#include <utility>

namespace vvvv {
template <typename T>
struct ScopedTrait;

template <typename T>
concept ScopedConcept = requires {
    typename ScopedTrait<T>::Owner;
} && requires(T& value, ScopedTrait<T>::Owner owner) {
    { ScopedTrait<T>::drop(value, owner) } -> std::same_as<void>;
};

template <typename T>
    requires ScopedConcept<T>
struct Scoped {
public:
    using Value = T;
    using Owner = ScopedTrait<T>::Owner;

public:
    Scoped() = default;

    explicit Scoped(Value value, Owner owner)
        : value_(std::move(value))
        , owner_(owner)
    {
    }

    ~Scoped()
    {
        ScopedTrait<T>::drop(value_, owner_);
    }

    Scoped(const Scoped&) = delete;
    Scoped& operator=(const Scoped&) = delete;

    Scoped(Scoped&& other) noexcept
        : value_(std::exchange(other.value_, {}))
        , owner_(std::exchange(other.owner_, {}))
    {
    }

    Scoped& operator=(Scoped&& other) noexcept
    {
        if (this != &other) {
            ScopedTrait<T>::drop(value_, owner_);
            value_ = std::exchange(other.value_, {});
            owner_ = std::exchange(other.owner_, {});
        }
        return *this;
    }

public:
    const T& value() const { return value_; }

    T release()
    {
        owner_ = {};
        return std::exchange(value_, {});
    }

private:
    Value value_ {};
    Owner owner_ {};
};
} // namespace vvvv
