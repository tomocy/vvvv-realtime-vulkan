#pragma once

#include <array>
#include <concepts>
#include <functional>
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
    std::invoke(std::forward<F>(f), v);
    return v;
}
} // namespace vvvv

namespace vvvv {
template <>
struct VkStructZeroTrait<VkApplicationInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
};

template <>
struct VkStructZeroTrait<VkInstanceCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkDebugUtilsMessengerCreateInfoEXT> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
};

template <>
struct VkStructZeroTrait<VkQueueFamilyProperties2> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
};

template <>
struct VkStructZeroTrait<VkPhysicalDeviceVulkan11Features> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
};

template <>
struct VkStructZeroTrait<VkPhysicalDeviceVulkan13Features> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
};

template <>
struct VkStructZeroTrait<VkDeviceCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkDeviceQueueCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkBufferCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkImageCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPhysicalDeviceMemoryProperties2> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
};

template <>
struct VkStructZeroTrait<VkMemoryRequirements2> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
};

template <>
struct VkStructZeroTrait<VkImageMemoryRequirementsInfo2> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
};

template <>
struct VkStructZeroTrait<VkMemoryAllocateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
};

template <>
struct VkStructZeroTrait<VkBindImageMemoryInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
};

template <>
struct VkStructZeroTrait<VkImageViewCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkCommandPoolCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkCommandBufferAllocateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
};

template <>
struct VkStructZeroTrait<VkCommandBufferBeginInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
};

template <>
struct VkStructZeroTrait<VkFenceCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkSubmitInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
};

template <>
struct VkStructZeroTrait<VkShaderModuleCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineLayoutCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineShaderStageCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineVertexInputStateCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineInputAssemblyStateCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineViewportStateCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineRasterizationStateCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineMultisampleStateCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineColorBlendStateCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkPipelineDynamicStateCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkGraphicsPipelineCreateInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
};

template <>
struct VkStructZeroTrait<VkRenderingInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
};

template <>
struct VkStructZeroTrait<VkRenderingAttachmentInfo> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
};

template <>
struct VkStructZeroTrait<VkBufferImageCopy2> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
};

template <>
struct VkStructZeroTrait<VkCopyImageToBufferInfo2> {
    static constexpr VkStructureType sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
};
} // namespace vvvv

namespace vvvv {
template <typename T>
concept VkBaseOutStructureConcept = requires(T v) {
    { v.sType } -> std::same_as<VkStructureType&>;
    { v.pNext } -> std::same_as<void*&>;
};
} // namespace vvvv

namespace vvvv {
struct ChainVkBaseOutStructures {
public:
    template <VkBaseOutStructureConcept... Ts>
    constexpr void* operator()(Ts&... vs) const noexcept
    {
        static_assert(sizeof...(vs) > 0);

        auto structs = std::to_array<VkBaseOutStructure*>({
            reinterpret_cast<VkBaseOutStructure*>(&vs)..., // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast
        });
        for (size_t i = 0; i < structs.size() - 1; ++i) {
            structs[i]->pNext = structs[i + 1];
        }
        structs.back()->pNext = nullptr;

        return structs.front();
    }
};
} // namespace vvvv
