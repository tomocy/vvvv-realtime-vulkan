#pragma once

#include <vulkan/vulkan_core.h>

#include "scoped.h"

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
