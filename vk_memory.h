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
struct ScopedTrait<VkDeviceMemory> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkDeviceMemory& memory, const Owner& owner)
    {
        if (memory == VK_NULL_HANDLE) {
            return;
        }

        vkFreeMemory(owner.device, memory, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct AllocateVkDeviceMemory {
public:
    Result::Either<Scoped<VkDeviceMemory>, Error> operator()() const noexcept
    {
        VkDeviceMemory memory = VK_NULL_HANDLE;
        const auto result = vkAllocateMemory(device, &info, allocator, &memory);
        if (result != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", result)));
        }

        return Result::OK(Scoped(memory, { .device = device, .allocator = allocator }));
    }

public:
    template <typename F>
        requires std::invocable<F&, AllocateVkDeviceMemory&>
        && std::same_as<std::invoke_result_t<F&, AllocateVkDeviceMemory&>, void>
    AllocateVkDeviceMemory& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, AllocateVkDeviceMemory&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkMemoryAllocateInfo info = vkStructZero<VkMemoryAllocateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
