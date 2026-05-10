#pragma once

#include <vulkan/vulkan_core.h>

#include "scoped.h"

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
