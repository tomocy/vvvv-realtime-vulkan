#pragma once

#include <format>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
template <>
struct ScopedTrait<VkImage> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkImage& image, const Owner& owner)
    {
        if (image == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyImage(owner.device, image, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkImage {
public:
    explicit CreateVkImage(VkDevice device)
        : device(device)
    {
    }

public:
    Result::Either<Scoped<VkImage>, Error> operator()() const noexcept
    {
        VkImage image = VK_NULL_HANDLE;
        const auto result = vkCreateImage(device, &info, allocator, &image);
        if (result != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", result)));
        }

        return Result::OK(Scoped(image, { .device = device, .allocator = allocator }));
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkImage&>
        && std::same_as<std::invoke_result_t<F&, CreateVkImage&>, void>
    CreateVkImage& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateVkImage&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkImageCreateInfo info = vkStructZero<VkImageCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv

namespace vvvv {
template <>
struct ScopedTrait<VkImageView> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkImageView& imageView, const Owner& owner)
    {
        if (imageView == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyImageView(owner.device, imageView, owner.allocator);
    }
};
} // namespace vvvv
