#pragma once

#include <concepts>
#include <utility>
#include <vulkan/vulkan_core.h>

namespace vvvv {
template <typename T>
struct VkStructZeroTrait;

template <typename T>
concept VkStructZeroConcept = requires(T v) {
    { v.sType } -> std::same_as<VkStructureType&>;
} && requires {
    { VkStructZeroTrait<T>::sType } -> std::convertible_to<VkStructureType>;
};

template <typename T, typename F = decltype([](T&) constexpr {})>
    requires VkStructZeroConcept<T>
    && std::invocable<F&, T&>
    && std::same_as<std::invoke_result_t<F&, T&>, void>
constexpr T vkStructZero(F&& f = {}) noexcept(std::is_nothrow_invocable_v<F&, T&>)
{
    T v {};
    v.sType = VkStructZeroTrait<T>::sType;
    std::forward<F>(f)(v);
    return v;
}
} // namespace vvvv

namespace vvvv {
template <>
struct VkStructZeroTrait<VkApplicationInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
};

template <>
struct VkStructZeroTrait<VkInstanceCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkDebugUtilsMessengerCreateInfoEXT> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
};

template <>
struct VkStructZeroTrait<VkQueueFamilyProperties2> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
};

template <>
struct VkStructZeroTrait<VkDeviceCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkDeviceQueueCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkCommandPoolCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkCommandBufferAllocateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
};
} // namespace vvvv
