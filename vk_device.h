#pragma once

#include <cstdint>
#include <format>
#include <span>
#include <tuple>
#include <type_traits>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "vk_queue.h"
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

namespace vvvv {
struct FindVkPhysicalDevice {
public:
    explicit FindVkPhysicalDevice(std::span<const VkPhysicalDevice> physicalDevices) noexcept
        : physicalDevices(physicalDevices)
    {
    }

public:
    [[nodiscard]] Result::Either<std::tuple<VkPhysicalDevice, uint32_t>, Error> invoke() const noexcept
    {
        for (const auto& device : physicalDevices) {
            const auto queueFamilyProps = vvvv::EnumerateVkQueueFamilyProperties(device).invoke();
            for (uint32_t i = 0; i < queueFamilyProps.size(); ++i) {
                const auto& props = queueFamilyProps[i];

                const auto hasQueue = (props.queueFamilyProperties.queueFlags & queueFlags) != 0U;
                if (hasQueue) {
                    return Result::OK(std::tuple { device, i });
                }
            }
        }

        return Result::Error(Error("No suitable physical device found"));
    }

public:
    template <typename F>
        requires std::is_nothrow_invocable_v<F&, FindVkPhysicalDevice&>
    FindVkPhysicalDevice& with(F f) noexcept
    {
        f(*this);
        return *this;
    }

public:
    std::span<const VkPhysicalDevice> physicalDevices;
    VkQueueFlags queueFlags = 0;
};
} // namespace vvvv
