#pragma once

#include <vulkan/vulkan_core.h>

#include "scoped.h"

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
