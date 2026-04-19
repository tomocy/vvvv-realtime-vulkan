#pragma once

#include <concepts>
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

template <typename T>
    requires VkStructZeroConcept<T>
constexpr T vkStructZero()
{
    T v {};
    v.sType = VkStructZeroTrait<T>::sType;
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
} // namespace vvvv
