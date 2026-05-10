#pragma once

#include <vector>
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

namespace vvvv {
template <>
struct ScopedTrait<VkCommandBuffer> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
    };

public:
    static void drop(VkCommandBuffer& commandBuffer, const Owner& owner)
    {
        if (commandBuffer == VK_NULL_HANDLE) {
            return;
        }

        vkFreeCommandBuffers(owner.device, owner.commandPool, 1, &commandBuffer);
    }
};

template <>
struct ScopedTrait<std::vector<VkCommandBuffer>> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
    };

public:
    static void drop(const std::vector<VkCommandBuffer>& commandBuffers, const Owner& owner)
    {
        if (commandBuffers.empty()) {
            return;
        }

        vkFreeCommandBuffers(owner.device, owner.commandPool, commandBuffers.size(), commandBuffers.data());
    }
};
} // namespace vvvv

namespace vvvv {
struct AllocateVkCommandBuffers {
public:
    explicit AllocateVkCommandBuffers(VkDevice device, VkCommandPool commandPool) noexcept
        : device(device)
        , commandPool(commandPool)
    {
    }

public:
    [[nodiscard]] Result::Either<Scoped<std::vector<VkCommandBuffer>>, Error> invoke() const noexcept
    {
        auto info = this->info;
        info.commandPool = commandPool;

        std::vector<VkCommandBuffer> commandBuffers(info.commandBufferCount, VK_NULL_HANDLE);
        const auto err = vkAllocateCommandBuffers(device, &info, commandBuffers.data());
        if (err != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", err)));
        }

        return Result::OK(Scoped(std::move(commandBuffers), { .device = device, .commandPool = commandPool }));
    }

public:
    template <typename F>
        requires std::invocable<F&, AllocateVkCommandBuffers&>
        && std::same_as<std::invoke_result_t<F&, AllocateVkCommandBuffers&>, void>
    AllocateVkCommandBuffers& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, AllocateVkCommandBuffers&>)
    {
        std::forward<F>(options)(*this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;

    VkCommandBufferAllocateInfo info = vkStructZero<VkCommandBufferAllocateInfo>([](auto& v) {
        v.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        v.commandBufferCount = 1;
    });

    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
