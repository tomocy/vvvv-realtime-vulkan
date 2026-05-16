#pragma once

#include <functional>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_command.h"
#include "vk_graphics.h"

namespace vvvv {
struct Renderer {
public:
    Renderer() = default;

    Renderer(
        VkDevice device,
        Scoped<VkCommandBuffer> commandBuffer,
        Scoped<VkFence> fence,
        VkGraphicsKernel triangleKernel
    )
        : device_(device)
        , commandBuffer_(std::move(commandBuffer))
        , fence_(std::move(fence))
        , triangleKernel_(std::move(triangleKernel))
    {
    }

public:
    [[nodiscard]] Error render(VkQueue queue) const noexcept
    {
        {
            const auto err = ResetVkCommandBuffer(commandBuffer_.value())();
            if (err.has()) {
                return Error::wrap("resetting VkCommandBuffer", err);
            }
        }
        {
            const auto fences = std::to_array({ fence_.value() });

            const auto err = ResetVkFences(device_)
                                 .with([&](auto& opts) {
                                     opts.fences = fences;
                                 })();
            if (err.has()) {
                return Error::wrap("resetting VkFence", err);
            }
        }
        {
            const auto renderExtent = VkExtent2D { .width = 512, .height = 512 };

            const auto record = [&](auto commandBuffer) {
                RecordVkDynamicRendering(
                    commandBuffer,
                    [&](auto commandBuffer) {
                        triangleKernel_.draw(commandBuffer, 3);
                    }
                ).with([&](auto& opts) {
                    opts.info.renderArea.extent = renderExtent;
                    opts.viewport.width = renderExtent.width;
                    opts.viewport.height = renderExtent.height;
                    opts.scissor.extent = renderExtent;
                })();
            };

            const auto err = RecordToVkCommandBuffer(commandBuffer_.value(), record)();
            if (err.has()) {
                return Error::wrap("recording to VkCommandBuffer", err);
            }
        }
        {
            const auto commandBuffers = std::to_array({ commandBuffer_.value() });

            const auto err = SubmitToVkQueue(queue)
                                 .with([&](auto& opts) {
                                     opts.info.commandBufferCount = commandBuffers.size();
                                     opts.info.pCommandBuffers = commandBuffers.data();
                                     opts.fence = fence_.value();
                                 })();
            if (err.has()) {
                return Error::wrap("submitting to VkQueue", err);
            }
        }
        {
            const auto fences = std::to_array({ fence_.value() });

            const auto err = WaitForVkFences(device_)
                                 .with([&](auto& opts) {
                                     opts.fences = fences;
                                 })();
            if (err.has()) {
                return Error::wrap("waiting for VkFence", err);
            }
        }

        return Error::none();
    }

private:
    VkDevice device_ = VK_NULL_HANDLE;
    Scoped<VkCommandBuffer> commandBuffer_;
    Scoped<VkFence> fence_;

    VkGraphicsKernel triangleKernel_;
};
} // namespace vvvv

namespace vvvv {
struct CreateRenderer {
public:
    Result::Either<Renderer, Error> operator()() const noexcept
    {
        Scoped<VkCommandBuffer> commandBuffer {};
        {
            auto r = AllocateVkCommandBuffers(device, commandPool)
                         .with([&](auto& opts) {
                             opts.allocator = allocator;
                         })();
            if (!r.isOK()) {
                return Result::Error(Error::wrap("allocating VkCommandBuffer", r.error()));
            }

            assert(r.ok().value().size() == 1);
            const auto vs = r.ok().release();
            commandBuffer = Scoped(vs[0], { .device = device, .commandPool = commandPool });
        }

        Scoped<VkFence> fence {};
        {
            auto r = CreateVkFence(device)();
            if (!r.isOK()) {
                return Result::Error(Error::wrap("creating VkFence", r.error()));
            }

            fence = std::move(r.ok());
        }

        VkGraphicsKernel triangleKernel {};
        {
            const auto dynamicState = std::to_array({
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
            });

            auto r = CreateVkGraphicsKernel(device).with([&](auto& opts) {
                opts.vertexShaderFilepath = "build/shader/triangle.vertex.spv";
                opts.fragmentShaderFilepath = "build/shader/triangle.fragment.spv";
                opts.viewportState.viewportCount = 1;
                opts.viewportState.scissorCount = 1;
                opts.dynamicState.dynamicStateCount = dynamicState.size();
                opts.dynamicState.pDynamicStates = dynamicState.data();
                opts.allocator = allocator;
            })();
            if (!r.isOK()) {
                return Result::Error(Error::wrap("creating VkGraphicsKernel for triangle", r.error()));
            }

            triangleKernel = std::move(r.ok());
        }

        return Result::OK(
            Renderer(
                device,
                std::move(commandBuffer),
                std::move(fence),
                std::move(triangleKernel)
            )
        );
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateRenderer&>
        && std::same_as<std::invoke_result_t<F&, CreateRenderer&>, void>
    CreateRenderer& with(F&& options) noexcept
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv
