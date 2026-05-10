#pragma once

#include <format>
#include <functional>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
template <>
struct ScopedTrait<VkDebugUtilsMessengerEXT> {
public:
    struct Owner {
        VkInstance instance = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = nullptr;
    };

public:
    static void drop(VkDebugUtilsMessengerEXT& messenger, const Owner& owner)
    {
        if (messenger == VK_NULL_HANDLE) {
            return;
        }

        owner.vkDestroyDebugUtilsMessenger(owner.instance, messenger, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkDebugUtilsMessenger {
public:
    explicit CreateVkDebugUtilsMessenger(VkInstance instance)
        : instance(instance)
    {
    }

public:
    [[nodiscard]] Result::Either<Scoped<VkDebugUtilsMessengerEXT>, Error> operator()() const noexcept
    {
        if (vkCreateDebugUtilsMessenger == nullptr) {
            return Result::Error(Error("vkCreateDebugUtilsMessenger not provided"));
        }
        if (vkDestroyDebugUtilsMessenger == nullptr) {
            return Result::Error(Error("vkDestroyDebugUtilsMessenger not provided"));
        }

        VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
        const auto result = vkCreateDebugUtilsMessenger(instance, &info, allocator, &messenger);
        if (result != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", result)));
        }

        return Result::OK(
            Scoped(
                messenger,
                {
                    .instance = instance,
                    .allocator = allocator,
                    .vkDestroyDebugUtilsMessenger = vkDestroyDebugUtilsMessenger,
                }
            )
        );
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkDebugUtilsMessenger&>
        && std::same_as<std::invoke_result_t<F&, CreateVkDebugUtilsMessenger&>, void>
    CreateVkDebugUtilsMessenger& with(F&& f)
    {
        std::invoke(std::forward<F>(f), *this);
        return *this;
    }

public:
    VkInstance instance = VK_NULL_HANDLE;

    VkDebugUtilsMessengerCreateInfoEXT info = vkStructZero<VkDebugUtilsMessengerCreateInfoEXT>();

    VkAllocationCallbacks* allocator = nullptr;

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = nullptr;
};
} // namespace vvvv
