#pragma once

#include <concepts>
#include <format>
#include <tuple>
#include <type_traits>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "scoped.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
template <>
struct ScopedTrait<VkInstance> {
public:
    struct Owner {
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkInstance& instance, const Owner& owner)
    {
        if (instance == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyInstance(instance, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkInstance {
public:
    [[nodiscard]] std::tuple<VkInstance, Error> invoke() const noexcept
    {
        VkInstance instance = VK_NULL_HANDLE;
        const auto result = vkCreateInstance(&info, allocator, &instance);
        if (result != VK_SUCCESS) {
            return { VK_NULL_HANDLE, vvvv::Error(std::format("{}", result)) };
        }

        return { instance, vvvv::Error::none() };
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkInstance&>
        && std::same_as<std::invoke_result_t<F&, CreateVkInstance&>, void>
    CreateVkInstance& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateVkInstance&>)
    {
        std::forward<F>(options)(*this);
        return *this;
    }

public:
    VkInstanceCreateInfo info = vkStructZero<VkInstanceCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
