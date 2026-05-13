#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "file.h"
#include "result.h"
#include "scoped.h"
#include "vk_pipeline.h"
#include "vk_shader.h"

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
    explicit CreateVkGraphicsKernel(VkDevice device)
        : device(device)
    {
    }

public:
    Result::Either<VkGraphicsKernel, Error> operator()() const
    {
        Scoped<VkPipelineLayout> pipelineLayout {};
        {
            auto r = CreateVkPipelineLayout(device)
                         .with([&](auto& opts) {
                             opts.allocator = allocator;
                         })();
            if (!r.isOK()) {
                return Result::Error(Error::wrap("creating VkPipelineLayout", r.error()));
            }

            pipelineLayout = std::move(r.ok());
        }

        Scoped<VkShaderModule> vertexShaderModule {};
        {
            std::vector<std::byte> code {};
            {
                auto r = FileReader().read(vertexShaderFilepath);
                if (!r.isOK()) {
                    return Result::Error(Error::wrap("reading vertex shader file", r.error()));
                }

                code = std::move(r.ok());
            }

            auto r = CreateVkShaderModule(device)
                         .with([&](auto& opts) {
                             opts.info.codeSize = code.size();
                             opts.info.pCode = reinterpret_cast<const uint32_t*>(code.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                             opts.allocator = allocator;
                         })();
            if (!r.isOK()) {
                return Result::Error(Error::wrap("creating vertex VkShaderModule", r.error()));
            }

            vertexShaderModule = std::move(r.ok());
        }

        Scoped<VkShaderModule> fragmentShaderModule {};
        {
            std::vector<std::byte> code {};
            {
                auto r = FileReader().read(fragmentShaderFilepath);
                if (!r.isOK()) {
                    return Result::Error(Error::wrap("reading fragment shader file", r.error()));
                }

                code = std::move(r.ok());
            }

            auto r = CreateVkShaderModule(device)
                         .with([&](auto& opts) {
                             opts.info.codeSize = code.size();
                             opts.info.pCode = reinterpret_cast<const uint32_t*>(code.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                             opts.allocator = allocator;
                         })();
            if (!r.isOK()) {
                return Result::Error(Error::wrap("creating fragment VkShaderModule", r.error()));
            }

            fragmentShaderModule = std::move(r.ok());
        }

        Scoped<VkPipeline> pipeline {};
        {
            const auto stages = std::to_array({
                vkStructZero<VkPipelineShaderStageCreateInfo>([&](auto& v) {
                    v.stage = VK_SHADER_STAGE_VERTEX_BIT;
                    v.module = vertexShaderModule.value();
                    v.pName = vertexShaderEntryPoint;
                }),
                vkStructZero<VkPipelineShaderStageCreateInfo>([&](auto& v) {
                    v.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                    v.module = fragmentShaderModule.value();
                    v.pName = fragmentShaderEntryPoint;
                }),
            });

            auto r = CreateVkGraphicsPipeline(device)
                         .with([&](auto& opts) {
                             opts.info.layout = pipelineLayout.value();
                             opts.info.stageCount = static_cast<uint32_t>(stages.size());
                             opts.info.pStages = stages.data();
                             opts.info.pVertexInputState = &vertexInputState;
                             opts.info.pInputAssemblyState = &inputAssemblyState;
                             opts.info.pViewportState = &viewportState;
                             opts.info.pRasterizationState = &rasterizationState;
                             opts.info.pMultisampleState = &multisampleState;
                             opts.info.pColorBlendState = &colorBlendState;
                             opts.info.pDynamicState = &dynamicState;
                             opts.allocator = allocator;
                         })();
            if (!r.isOK()) {
                return Result::Error(Error::wrap("creating VkGraphicsPipeline", r.error()));
            }

            pipeline = std::move(r.ok());
        }

        return Result::OK(
            VkGraphicsKernel(
                std::move(pipelineLayout),
                std::move(pipeline)
            )
        );
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkGraphicsKernel&>
        && std::same_as<std::invoke_result_t<F&, CreateVkGraphicsKernel&>, void>
    CreateVkGraphicsKernel& with(F&& f) noexcept(std::is_nothrow_invocable_v<F&, CreateVkGraphicsKernel&>)
    {
        std::invoke(std::forward<F>(f), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;

    std::filesystem::path vertexShaderFilepath;
    const char* vertexShaderEntryPoint = "main";

    std::filesystem::path fragmentShaderFilepath;
    const char* fragmentShaderEntryPoint = "main";

    VkPipelineVertexInputStateCreateInfo vertexInputState = vkStructZero<VkPipelineVertexInputStateCreateInfo>();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = vkStructZero<VkPipelineInputAssemblyStateCreateInfo>([](auto& v) {
        v.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    });

    VkPipelineViewportStateCreateInfo viewportState = vkStructZero<VkPipelineViewportStateCreateInfo>();

    VkPipelineRasterizationStateCreateInfo rasterizationState = vkStructZero<VkPipelineRasterizationStateCreateInfo>([](auto& v) {
        v.lineWidth = 1.0;
    });

    VkPipelineMultisampleStateCreateInfo multisampleState = vkStructZero<VkPipelineMultisampleStateCreateInfo>([](auto& v) {
        v.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    });

    VkPipelineColorBlendStateCreateInfo colorBlendState = vkStructZero<VkPipelineColorBlendStateCreateInfo>();
    VkPipelineDynamicStateCreateInfo dynamicState = vkStructZero<VkPipelineDynamicStateCreateInfo>();

    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
