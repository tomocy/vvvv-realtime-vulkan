#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "vk_struct.h"

namespace vvvv {
struct EnumerateVkQueueFamilyProperties {
public:
    explicit EnumerateVkQueueFamilyProperties(VkPhysicalDevice physicalDevice) noexcept
        : physicalDevice(physicalDevice)
    {
    }

public:
    [[nodiscard]] std::vector<VkQueueFamilyProperties2> invoke() const noexcept
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &count, nullptr);

        std::vector<VkQueueFamilyProperties2> props(count, vkStructZero<VkQueueFamilyProperties2>());
        vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &count, props.data());

        return props;
    }

public:
    template <typename F>
        requires std::is_nothrow_invocable_v<F&, EnumerateVkQueueFamilyProperties&>
    EnumerateVkQueueFamilyProperties& with(F f) noexcept
    {
        f(*this);
        return *this;
    }

public:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
};
} // namespace vvvv
