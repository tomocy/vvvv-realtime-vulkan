#pragma once

#include <functional>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "vk_struct.h"

namespace vvvv {
struct CopyVkImageToVkBuffer {
public:
    explicit CopyVkImageToVkBuffer(VkCommandBuffer commandBuffer) noexcept
        : commandBuffer(commandBuffer)
    {
    }

public:
    [[nodiscard]] Error operator()() const noexcept
    {
        vkCmdCopyImageToBuffer2(commandBuffer, &info);
        return Error::none();
    }

public:
    template <typename F>
        requires std::invocable<F&, CopyVkImageToVkBuffer&>
        && std::same_as<std::invoke_result_t<F&, CopyVkImageToVkBuffer&>, void>
    CopyVkImageToVkBuffer& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CopyVkImageToVkBuffer&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkCopyImageToBufferInfo2 info = vkStructZero<VkCopyImageToBufferInfo2>();
};
} // namespace vvvv
