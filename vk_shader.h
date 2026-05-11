#pragma once

#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
template <>
struct ScopedTrait<VkShaderModule> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkShaderModule& shaderModule, const Owner& owner)
    {
        if (shaderModule == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyShaderModule(owner.device, shaderModule, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkShaderModule {
public:
    explicit CreateVkShaderModule(VkDevice device)
        : device(device)
    {
    }

public:
    Result::Either<Scoped<VkShaderModule>, Error> operator()() const
    {
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        const auto result = vkCreateShaderModule(device, &info, allocator, &shaderModule);
        if (result != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", result)));
        }

        return Result::OK(Scoped(shaderModule, { device, allocator }));
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkShaderModule&>
        && std::same_as<std::invoke_result_t<F&, CreateVkShaderModule&>, void>
    CreateVkShaderModule& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateVkShaderModule&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo info = vkStructZero<VkShaderModuleCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
