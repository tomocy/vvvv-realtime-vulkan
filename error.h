#pragma once

#include <format>
#include <string>
#include <utility>

namespace vvvv {
struct Error {
public:
    static Error none() noexcept { return {}; }

    static Error wrap(const std::string_view label, const Error& err)
    {
        return Error(std::format("{}: {}", label, err.message()));
    }

public:
    Error() = default;

    explicit Error(std::string message) noexcept
        : message_(std::move(message))
    {
    }

public:
    [[nodiscard]] bool has() const noexcept { return !message_.empty(); }

public:
    [[nodiscard]] const std::string& message() const noexcept { return message_; }

private:
    std::string message_ = "";
};
} // namespace vvvv
