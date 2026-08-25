#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"

namespace vvvv {
struct Capturer {
public:
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    [[nodiscard]] Result::Either<std::vector<std::byte>, Error> capture(
        const uint32_t width,
        const uint32_t height,
        const uint32_t bytesPerPixel
    ) const noexcept
    {
        const std::vector<std::byte> data(
            static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(bytesPerPixel),
            std::byte { 0 }
        );

        return Result::OK<std::vector<std::byte>>(data);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateCapturer {
public:
    Result::Either<Capturer, Error> operator()() const noexcept
    {
        return Result::OK(Capturer());
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateCapturer&>
        && std::same_as<std::invoke_result_t<F&, CreateCapturer&>, void>
    CreateCapturer with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateCapturer&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }
};
} // namespace vvvv
