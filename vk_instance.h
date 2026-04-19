#pragma once

#include <format>
#include <functional>
#include <span>
#include <tuple>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
struct CreateVkInstance {
public:
    [[nodiscard]] std::tuple<VkInstance, Error> invoke() const noexcept
    {
        const auto createInfo = [&]() {
            auto v = vkStructZero<VkInstanceCreateInfo>();
            v.pNext = next;
            v.flags = flags;
            v.pApplicationInfo = &appInfo;
            v.enabledLayerCount = layerNames.size();
            v.ppEnabledLayerNames = layerNames.data();
            v.enabledExtensionCount = extensionNames.size();
            v.ppEnabledExtensionNames = extensionNames.data();
            return v;
        }();

        VkInstance instance = VK_NULL_HANDLE;
        const auto result = vkCreateInstance(&createInfo, allocator, &instance);
        if (result != VK_SUCCESS) {
            return { VK_NULL_HANDLE, vvvv::Error(std::format("{}", result)) };
        }

        return { instance, vvvv::Error::none() };
    }

public:
    CreateVkInstance& with(const std::function<void(CreateVkInstance&)>& options) noexcept
    {
        options(*this);
        return *this;
    }

public:
    const void* next = nullptr;
    VkInstanceCreateFlags flags = 0;
    VkApplicationInfo appInfo = vkStructZero<VkApplicationInfo>();
    std::span<const char* const> layerNames;
    std::span<const char* const> extensionNames;

    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
