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
#include "scoped.h"
#include "vk_queue.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
struct EnumerateVkPhysicalDevices {
public:
    explicit EnumerateVkPhysicalDevices(VkInstance instance) noexcept
        : instance(instance)
    {
    }

public:
    [[nodiscard]] Result::Either<std::vector<VkPhysicalDevice>, Error> operator()() const noexcept
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
    [[nodiscard]] Result::Either<std::tuple<VkPhysicalDevice, uint32_t>, Error> operator()() const noexcept
    {
        for (const auto& device : physicalDevices) {
            const auto queueFamilyProps = vvvv::EnumerateVkQueueFamilyProperties(device)();
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

namespace vvvv {
template <>
struct ScopedTrait<VkDevice> {
public:
    struct Owner {
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkDevice& device, const Owner& owner)
    {
        if (device == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyDevice(device, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkDevice {
public:
    explicit CreateVkDevice(VkPhysicalDevice physicalDevice) noexcept
        : physicalDevice(physicalDevice)
    {
    }

public:
    [[nodiscard]] Result::Either<Scoped<VkDevice>, Error> operator()() const noexcept
    {
        VkDevice device = VK_NULL_HANDLE;
        const auto result = vkCreateDevice(physicalDevice, &info, allocator, &device);
        if (result != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", result)));
        }

        return Result::OK(Scoped(device, { .allocator = allocator }));
    }

public:
    template <typename F>
        requires std::is_nothrow_invocable_v<F&, CreateVkDevice&>
    CreateVkDevice& with(F f) noexcept
    {
        f(*this);
        return *this;
    }

public:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDeviceCreateInfo info = vkStructZero<VkDeviceCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
