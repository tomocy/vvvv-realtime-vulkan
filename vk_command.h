#pragma once

#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
template <>
struct ScopedTrait<VkCommandPool> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkCommandPool& commandPool, const Owner& owner)
    {
        if (commandPool == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyCommandPool(owner.device, commandPool, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkCommandPool {
public:
    explicit CreateVkCommandPool(VkDevice device) noexcept
        : device(device)
    {
    }

public:
    [[nodiscard]] Result::Either<Scoped<VkCommandPool>, Error> invoke() const noexcept
    {
        VkCommandPool commandPool = VK_NULL_HANDLE;
        const auto err = vkCreateCommandPool(device, &info, allocator, &commandPool);
        if (err != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", err)));
        }

        return Result::OK(Scoped<VkCommandPool>(commandPool, { .device = device, .allocator = allocator }));
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkCommandPool&>
        && std::same_as<std::invoke_result_t<F&, CreateVkCommandPool&>, void>
    CreateVkCommandPool& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateVkCommandPool&>)
    {
        std::forward<F>(options)(*this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo info = vkStructZero<VkCommandPoolCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
