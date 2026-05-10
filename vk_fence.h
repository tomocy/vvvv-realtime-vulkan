#pragma once

#include <concepts>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
template <>
struct ScopedTrait<VkFence> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkFence& fence, const Owner& owner)
    {
        if (fence == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyFence(owner.device, fence, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkFence {
public:
    explicit CreateVkFence(VkDevice device)
        : device(device)
    {
    }

public:
    [[nodiscard]] Result::Either<Scoped<VkFence>, Error> invoke() const noexcept
    {
        VkFence fence = VK_NULL_HANDLE;
        const auto err = vkCreateFence(device, &info, allocator, &fence);
        if (err != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", err)));
        }

        return Result::OK(Scoped(fence, { .device = device, .allocator = allocator }));
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkFence&>
        && std::same_as<std::invoke_result_t<F&, CreateVkFence&>, void>
    CreateVkFence& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateVkFence&>)
    {
        std::forward<F>(options)(*this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkFenceCreateInfo info = vkStructZero<VkFenceCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
