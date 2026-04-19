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
} // namespace vvvv
