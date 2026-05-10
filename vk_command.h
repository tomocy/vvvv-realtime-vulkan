#pragma once

#include <array>
#include <functional>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "result.h"
#include "scoped.h"
#include "vk_fence.h"
#include "vk_queue.h"
#include "vk_result.h" // IWYU pragma: keep
#include "vk_struct.h"

namespace vvvv {
template <>
struct ScopedTrait<VkCommandPool> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkAllocationCallbacks* allocator = nullptr;
    };

public:
    static void drop(VkCommandPool& commandPool, const Owner& owner)
    {
        if (commandPool == VK_NULL_HANDLE) {
            return;
        }

        vkDestroyCommandPool(owner.device, commandPool, owner.allocator);
    }
};
} // namespace vvvv

namespace vvvv {
struct CreateVkCommandPool {
public:
    explicit CreateVkCommandPool(VkDevice device) noexcept
        : device(device)
    {
    }

public:
    [[nodiscard]] Result::Either<Scoped<VkCommandPool>, Error> invoke() const noexcept
    {
        VkCommandPool commandPool = VK_NULL_HANDLE;
        const auto err = vkCreateCommandPool(device, &info, allocator, &commandPool);
        if (err != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", err)));
        }

        return Result::OK(Scoped<VkCommandPool>(commandPool, { .device = device, .allocator = allocator }));
    }

public:
    template <typename F>
        requires std::invocable<F&, CreateVkCommandPool&>
        && std::same_as<std::invoke_result_t<F&, CreateVkCommandPool&>, void>
    CreateVkCommandPool& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, CreateVkCommandPool&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo info = vkStructZero<VkCommandPoolCreateInfo>();
    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv

namespace vvvv {
template <>
struct ScopedTrait<VkCommandBuffer> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
    };

public:
    static void drop(VkCommandBuffer& commandBuffer, const Owner& owner)
    {
        if (commandBuffer == VK_NULL_HANDLE) {
            return;
        }

        vkFreeCommandBuffers(owner.device, owner.commandPool, 1, &commandBuffer);
    }
};

template <>
struct ScopedTrait<std::vector<VkCommandBuffer>> {
public:
    struct Owner {
        VkDevice device = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
    };

public:
    static void drop(const std::vector<VkCommandBuffer>& commandBuffers, const Owner& owner)
    {
        if (commandBuffers.empty()) {
            return;
        }

        vkFreeCommandBuffers(owner.device, owner.commandPool, commandBuffers.size(), commandBuffers.data());
    }
};
} // namespace vvvv

namespace vvvv {
struct AllocateVkCommandBuffers {
public:
    explicit AllocateVkCommandBuffers(VkDevice device, VkCommandPool commandPool) noexcept
        : device(device)
        , commandPool(commandPool)
    {
    }

public:
    [[nodiscard]] Result::Either<Scoped<std::vector<VkCommandBuffer>>, Error> invoke() const noexcept
    {
        auto info = this->info;
        info.commandPool = commandPool;

        std::vector<VkCommandBuffer> commandBuffers(info.commandBufferCount, VK_NULL_HANDLE);
        const auto err = vkAllocateCommandBuffers(device, &info, commandBuffers.data());
        if (err != VK_SUCCESS) {
            return Result::Error(Error(std::format("{}", err)));
        }

        return Result::OK(Scoped(std::move(commandBuffers), { .device = device, .commandPool = commandPool }));
    }

public:
    template <typename F>
        requires std::invocable<F&, AllocateVkCommandBuffers&>
        && std::same_as<std::invoke_result_t<F&, AllocateVkCommandBuffers&>, void>
    AllocateVkCommandBuffers& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, AllocateVkCommandBuffers&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;

    VkCommandBufferAllocateInfo info = vkStructZero<VkCommandBufferAllocateInfo>([](auto& v) {
        v.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        v.commandBufferCount = 1;
    });

    VkAllocationCallbacks* allocator = nullptr;
};
} // namespace vvvv

namespace vvvv {
template <typename R>
    requires std::invocable<const R&, VkCommandBuffer>
    && std::same_as<std::invoke_result_t<const R&, VkCommandBuffer>, void>
struct RecordToVkCommandBuffer {
public:
    explicit RecordToVkCommandBuffer(VkCommandBuffer commandBuffer, R record) noexcept
        : commandBuffer(commandBuffer)
        , record(std::move(record))
    {
    }

public:
    [[nodiscard]] Error invoke() const noexcept(std::is_nothrow_invocable_v<const R&, VkCommandBuffer>)
    {
        {
            const auto err = vkBeginCommandBuffer(commandBuffer, &beginInfo);
            if (err != VK_SUCCESS) {
                return Error(std::format("{}", err));
            }
        }

        std::invoke(record, commandBuffer);

        {
            const auto err = vkEndCommandBuffer(commandBuffer);
            if (err != VK_SUCCESS) {
                return Error(std::format("{}", err));
            }
        }

        return Error::none();
    }

public:
    template <typename F>
        requires std::invocable<F&, RecordToVkCommandBuffer&>
        && std::same_as<std::invoke_result_t<F&, RecordToVkCommandBuffer&>, void>
    RecordToVkCommandBuffer& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, RecordToVkCommandBuffer&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkCommandBufferBeginInfo beginInfo = vkStructZero<VkCommandBufferBeginInfo>();
    R record;
};
} // namespace vvvv

namespace vvvv {
template <typename R>
    requires std::invocable<const R&, VkCommandBuffer>
    && std::same_as<std::invoke_result_t<const R&, VkCommandBuffer>, void>
struct AllocateRecordToVkCommandBuffer {
public:
    AllocateRecordToVkCommandBuffer(VkDevice device, VkCommandPool commandPool, R record) noexcept
        : device(device)
        , commandPool(commandPool)
        , record(std::move(record))
    {
    }

public:
    [[nodiscard]] Result::Either<Scoped<VkCommandBuffer>, Error> invoke() const noexcept(std::is_nothrow_invocable_v<const R&, VkCommandBuffer>)
    {
        Scoped<VkCommandBuffer> commandBuffer {};
        {
            auto r = AllocateVkCommandBuffers(device, commandPool)
                         .with([&](auto& opts) {
                             opts.info.commandBufferCount = 1;
                         })
                         .invoke();
            if (!r.isOK()) {
                return Result::Error(Error::wrap("allocating VkCommandBuffer", r.error()));
            }

            assert(r.ok().value().size() == 1);
            const auto vs = r.ok().release();
            commandBuffer = Scoped(vs[0], { .device = device, .commandPool = commandPool });
        }

        {
            const auto err = RecordToVkCommandBuffer(commandBuffer.value(), record)
                                 .with([&](auto& opts) {
                                     opts.beginInfo = beginInfo;
                                 })
                                 .invoke();
            if (err.has()) {
                return Result::Error(std::move(err));
            }
        }

        return Result::OK(std::move(commandBuffer));
    }

public:
    template <typename F>
        requires std::invocable<F&, AllocateRecordToVkCommandBuffer&>
        && std::same_as<std::invoke_result_t<F&, AllocateRecordToVkCommandBuffer&>, void>
    AllocateRecordToVkCommandBuffer& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, AllocateRecordToVkCommandBuffer&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandBufferBeginInfo beginInfo = vkStructZero<VkCommandBufferBeginInfo>();
    R record;
};
} // namespace vvvv

namespace vvvv {
struct ExecuteVkCommandBuffers {
public:
    explicit ExecuteVkCommandBuffers(VkDevice device, VkQueue queue) noexcept
        : device(device)
        , queue(queue)
    {
    }

public:
    [[nodiscard]] Error invoke() const noexcept
    {
        Scoped<VkFence> fence {};
        {
            auto r = CreateVkFence(device).invoke();
            if (!r.isOK()) {
                return Error::wrap("creating VkFence", r.error());
            }

            fence = std::move(r.ok());
        }

        {
            const auto err = SubmitToVkQueue(queue)
                                 .with([&](auto& opts) {
                                     opts.info.commandBufferCount = commandBuffers.size();
                                     opts.info.pCommandBuffers = commandBuffers.data();
                                     opts.fence = fence.value();
                                 })
                                 .invoke();
            if (err.has()) {
                return Error::wrap("submitting to VkQueue", err);
            }
        }
        {
            const auto fences = std::to_array({ fence.value() });

            const auto err = WaitForVkFences(device)
                                 .with([&](auto& opts) {
                                     opts.fences = fences;
                                 })
                                 .invoke();
            if (err.has()) {
                return Error::wrap("waiting for VkFence", err);
            }
        }

        return Error::none();
    }

public:
    template <typename F>
        requires std::invocable<F&, ExecuteVkCommandBuffers&>
        && std::same_as<std::invoke_result_t<F&, ExecuteVkCommandBuffers&>, void>
    ExecuteVkCommandBuffers& with(F&& options) noexcept(std::is_nothrow_invocable_v<F&, ExecuteVkCommandBuffers&>)
    {
        std::invoke(std::forward<F>(options), *this);
        return *this;
    }

public:
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    std::span<const VkCommandBuffer> commandBuffers;
};
} // namespace vvvv
