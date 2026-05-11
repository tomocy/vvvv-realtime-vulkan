#pragma once

#include <vulkan/vulkan_core.h>

#include "scoped.h"

namespace vvvv {
template <>
struct ScopedTrait<VkShaderModule> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkShaderModule& shaderModule, const Owner& owner)
    {
        if (shaderModule == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyShaderModule(owner.device, shaderModule, owner.allocator);
    }
};
} // namespace vvvv
