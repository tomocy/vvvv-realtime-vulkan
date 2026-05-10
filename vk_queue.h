#pragma once

#include <concepts>
#include <functional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "vk_result.h" // IWYU pragma: keep
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

namespace vvvv {
struct SubmitToVkQueue {
public:
    explicit SubmitToVkQueue(VkQueue queue) noexcept
        : queue(queue)
    {
    }

public:
    [[nodiscard]] Error invoke() const noexcept
    {
        const auto err = vkQueueSubmit(queue, 1, &info, fence);
        if (err != VK_SUCCESS) {
            return Error(std::format("{}", err));
        }

        return Error::none();
    }

public:
    template <typename F>
        requires std::invocable<F&, SubmitToVkQueue&>
        && std::same_as<std::invoke_result_t<F&, SubmitToVkQueue&>, void>
    SubmitToVkQueue& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, SubmitToVkQueue&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkQueue queue = VK_NULL_HANDLE;
    VkSubmitInfo info = vkStructZero<VkSubmitInfo>();
    VkFence fence = VK_NULL_HANDLE;
};
} // namespace vvvv
