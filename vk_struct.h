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
