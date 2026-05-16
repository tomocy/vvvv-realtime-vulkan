#pragma once

#include <vulkan/vulkan_core.h>

#include "scoped.h"

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
