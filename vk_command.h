#pragma once

#include <vulkan/vulkan_core.h>

#include "scoped.h"

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
