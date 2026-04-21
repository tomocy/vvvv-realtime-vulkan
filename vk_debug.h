#pragma once

#include <vulkan/vulkan_core.h>

#include "scoped.h"

namespace vvvv {
template <>
struct ScopedTrait<VkDebugUtilsMessengerEXT> {
public:
    struct Owner {
        VkInstance instance = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = nullptr;
    };

public:
    static void drop(VkDebugUtilsMessengerEXT& messenger, const Owner& owner)
    {
        if (messenger == VK_NULL_HANDLE) {
            return;
        }

        owner.vkDestroyDebugUtilsMessenger(owner.instance, messenger, owner.allocator);
    }
};
} // namespace vvvv
