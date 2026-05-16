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
struct ScopedTrait<VkBuffer> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkBuffer& buffer, const Owner& owner)
    {
        if (buffer == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyBuffer(owner.device, buffer, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkBuffer {
public:
    explicit CreateVkBuffer(VkDevice device)
        : device(device)
    {
    }

public:
    Result::Either<Scoped<VkBuffer>, Error> operator()() const noexcept
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        const auto result = vkCreateBuffer(device, &info, allocator, &buffer);
        if (result != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", result)));
        }

        return Result::OK(Scoped(buffer, { .device = device, .allocator = allocator }));
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkBuffer&>
        && std::same_as<std::invoke_result_t<F&, CreateVkBuffer&>, void>
    CreateVkBuffer&
    with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateVkBuffer&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkBufferCreateInfo info = vkStructZero<VkBufferCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
