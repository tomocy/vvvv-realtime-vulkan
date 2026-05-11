#pragma once

#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_pipeline.h" // IWYU pragma: keep

namespace vvvv {
struct VkGraphicsKernel {
public:
    VkGraphicsKernel() = default;

    VkGraphicsKernel(Scoped<VkPipelineLayout> pipelineLayout, Scoped<VkPipeline> pipeline)
        : pipelineLayout_(std::move(pipelineLayout))
        , pipeline_(std::move(pipeline))
    {
    }

public:
    void draw(
        VkCommandBuffer commandBuffer,
        const uint32_t vertexCount,
        const uint32_t instanceCount = 1,
        const uint32_t firstVertex = 0,
        const uint32_t firstInstance = 0
    ) const noexcept
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.value());
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

private:
    Scoped<VkPipelineLayout> pipelineLayout_;
    Scoped<VkPipeline> pipeline_;
};
} // namespace vvvv

namespace vvvv {
struct CreateVkGraphicsKernel {
public:
    Result::Either<VkGraphicsKernel, Error> operator()() const
    {
        return Result::OK(VkGraphicsKernel());
    }
};
} // namespace vvvv
