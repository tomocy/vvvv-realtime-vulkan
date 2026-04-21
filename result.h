#pragma once

#include <cassert>
#include <utility>
#include <variant>

namespace vvvv {
struct Result {
    template <typename T>
    struct OK {
    public:
        explicit OK(T v)
            : value(std::move(v))
        {
        }

    public:
        T value {};
    };

    template <typename T>
    struct Error {
    public:
        explicit Error(T v)
            : value(std::move(v))
        {
        }

    public:
        T value {};
    };

    template <typename V, typename E>
    struct Either {
    public:
        // NOLINTNEXTLINE(google-explicit-constructor)
        Either(OK<V> ok)
            : data_(std::in_place_type<V>, std::move(ok.value))
        {
        }

        // NOLINTNEXTLINE(google-explicit-constructor)
        Either(Error<E> err)
            : data_(std::in_place_type<E>, std::move(err.value))
        {
        }

    public:
        [[nodiscard]] bool isOK() const noexcept
        {
            return std::holds_alternative<V>(data_);
        }

        V& ok() noexcept
        {
            assert(isOK());
            return std::get<V>(data_);
        }
        const V& ok() const noexcept
        {
            assert(isOK());
            return std::get<V>(data_);
        }

        E& error() noexcept
        {
            assert(!isOK());
            return std::get<E>(data_);
        }
        const E& error() const noexcept
        {
            assert(!isOK());
            return std::get<E>(data_);
        }

    private:
        std::variant<V, E> data_ {};
    };
};
} // namespace vvvv
