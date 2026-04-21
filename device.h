#pragma once

#include <format>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "vk_result.h" // IWYU pragma: keep

namespace vvvv {
struct EnumerateVkPhysicalDevices {
public:
    explicit EnumerateVkPhysicalDevices(VkInstance instance) noexcept
        : instance(instance)
    {
    }

public:
    [[nodiscard]] Result::Either<std::vector<VkPhysicalDevice>, Error> invoke() const noexcept
    {
        uint32_t count = 0;
        {
            const auto result = vkEnumeratePhysicalDevices(instance, &count, nullptr);
            if (result != VK_SUCCESS) {
                return Result::Error(Error(std::format("{}", result)));
            }
        }

        std::vector<VkPhysicalDevice> devices(count);
        {
            const auto result = vkEnumeratePhysicalDevices(instance, &count, devices.data());
            if (result != VK_SUCCESS) {
                return Result::Error(Error(std::format("{}", result)));
            }
        }

        return Result::OK(devices);
    }

public:
    VkInstance instance = VK_NULL_HANDLE;
};
} // namespace vvvv
