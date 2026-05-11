#pragma once

#include <vulkan/vulkan_core.h>

#include "scoped.h"

namespace vvvv {
template <>
struct ScopedTrait<VkPipelineLayout> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkPipelineLayout& pipelineLayout, const Owner& owner)
    {
        if (pipelineLayout == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyPipelineLayout(owner.device, pipelineLayout, owner.allocator);
    }
};
} // namespace vvvv
