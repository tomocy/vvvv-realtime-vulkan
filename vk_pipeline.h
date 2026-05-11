#pragma once

#include <format>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
template <>
struct ScopedTrait<VkPipelineLayout> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkPipelineLayout& pipelineLayout, const Owner& owner)
    {
        if (pipelineLayout == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyPipelineLayout(owner.device, pipelineLayout, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkPipelineLayout {
public:
    explicit CreateVkPipelineLayout(VkDevice device)
        : device(device)
    {
    }

public:
    Result::Either<Scoped<VkPipelineLayout>, Error> operator()() const
    {
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        const auto result = vkCreatePipelineLayout(device, &info, allocator, &pipelineLayout);
        if (result != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", result)));
        }

        return Result::OK(Scoped(pipelineLayout, { device, allocator }));
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkPipelineLayout&>
        && std::same_as<std::invoke_result_t<F&, CreateVkPipelineLayout&>, void>
    CreateVkPipelineLayout& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateVkPipelineLayout&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo info = vkStructZero<VkPipelineLayoutCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv

namespace vvvv {
template <>
struct ScopedTrait<VkPipeline> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkPipeline& pipeline, const Owner& owner)
    {
        if (pipeline == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyPipeline(owner.device, pipeline, owner.allocator);
    }
};
} // namespace vvvv
