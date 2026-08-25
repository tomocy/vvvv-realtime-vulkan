#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "bmp.h"
#include "capturer.h"
#include "error.h"
#include "file.h"
#include "renderer.h"
#include "scoped.h"
#include "vk_command.h"
#include "vk_debug.h"
#include "vk_device.h"
#include "vk_image.h"
#include "vk_instance.h"
#include "vk_memory.h"
#include "vk_pfn.h"
#include "vk_struct.h"

namespace {
vvvv::Error run();
} // namespace

int main()
{
    const auto err = run();
    if (err.has()) {
        std::cerr << "Error: " << err.message() << "\n";
        return 1;
    }
    return 0;
}

namespace {
struct VkDispatchTable {
public:
    vvvv::Error resolveFrom(VkInstance instance) noexcept
    {
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
        const auto entries = std::to_array<std::pair<PFN_vkVoidFunction*, const char*>>({
            std::pair { reinterpret_cast<PFN_vkVoidFunction*>(&vkCreateDebugUtilsMessenger), "vkCreateDebugUtilsMessengerEXT" },
            std::pair { reinterpret_cast<PFN_vkVoidFunction*>(&vkDestroyDebugUtilsMessenger), "vkDestroyDebugUtilsMessengerEXT" },
        });
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

        for (const auto& [dst, name] : entries) {
            const auto addr = vvvv::GetVkProcessAddress<VkInstance>(instance)
                                  .with([&](auto& opts) { opts.name = name; })
                                  .operator()<PFN_vkVoidFunction>();
            if (addr == nullptr) {
                return vvvv::Error(std::format("{} not available", name));
            }

            *dst = addr;
        }

        return vvvv::Error::none();
    }

public:
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = nullptr;
};
} // namespace

namespace {
VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT types,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* userData
);
} // namespace

namespace {
vvvv::Error run()
{
    VkAllocationCallbacks* allocator = nullptr;

    vvvv::Scoped<VkInstance> instance {};
    {
        const auto appInfo = vvvv::vkStructZero<VkApplicationInfo>([](auto& v) {
            v.pApplicationName = "vvvv";
            v.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);
        });

        const auto layerNames = std::to_array({
            "VK_LAYER_KHRONOS_validation",
        });
        const auto extensionNames = std::to_array({
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        });

        auto r = vvvv::CreateVkInstance()
                     .with([&](auto& opts) {
                         opts.info.pApplicationInfo = &appInfo;
                         opts.info.ppEnabledLayerNames = layerNames.data();
                         opts.info.enabledLayerCount = layerNames.size();
                         opts.info.ppEnabledExtensionNames = extensionNames.data();
                         opts.info.enabledExtensionCount = extensionNames.size();
                         opts.allocator = allocator;
                     })();
        if (!r.isOK()) {
            return vvvv::Error::wrap("creating VkInstance", r.error());
        }

        instance = std::move(r.ok());
        std::cout << "VkInstance: " << instance.value() << "\n";

        std::cout << "VkInstance layers:\n";
        for (const auto& name : layerNames) {
            std::cout << "- " << name << "\n";
        }

        std::cout << "VkInstance extensions:\n";
        for (const auto& name : extensionNames) {
            std::cout << "- " << name << "\n";
        }
    }

    VkDispatchTable dispatchTable {};
    {
        const auto err = dispatchTable.resolveFrom(instance.value());
        if (err.has()) {
            return vvvv::Error::wrap("resolving VkInstance dispatch table", err);
        }
    }

    vvvv::Scoped<VkDebugUtilsMessengerEXT> debugMessenger {};
    {
        auto r = vvvv::CreateVkDebugUtilsMessenger(instance.value())
                     .with([&](auto& opts) {
                         opts.info.messageSeverity = static_cast<VkFlags>(0)
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                         opts.info.messageType = static_cast<VkFlags>(0)
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
                         opts.info.pfnUserCallback = vkDebugUtilsMessengerCallback;
                         opts.allocator = allocator;
                         opts.vkCreateDebugUtilsMessenger = dispatchTable.vkCreateDebugUtilsMessenger;
                         opts.vkDestroyDebugUtilsMessenger = dispatchTable.vkDestroyDebugUtilsMessenger;
                     })();
        if (!r.isOK()) {
            return vvvv::Error::wrap("creating VkDebugUtilsMessenger", r.error());
        }

        debugMessenger = std::move(r.ok());
        std::cout << "VkDebugUtilsMessenger: " << debugMessenger.value() << "\n";
    }

    VkPhysicalDevice physicalDevice {};
    uint32_t queueFamilyIndex {};
    {
        std::vector<VkPhysicalDevice> physicalDevices {};
        {
            auto r = vvvv::EnumerateVkPhysicalDevices(instance.value())();
            if (!r.isOK()) {
                return vvvv::Error::wrap("enumerating VkPhysicalDevices", r.error());
            }

            physicalDevices = std::move(r.ok());
        }
        {
            auto r = vvvv::FindVkPhysicalDevice(physicalDevices)
                         .with([](auto& opts) noexcept {
                             opts.queueFlags = VK_QUEUE_GRAPHICS_BIT;
                         })();
            if (!r.isOK()) {
                return vvvv::Error::wrap("finding VkPhysicalDevice", r.error());
            }

            std::tie(physicalDevice, queueFamilyIndex) = r.ok();
        }

        std::cout << "VkPhysicalDevice: " << physicalDevice << "\n";
        std::cout << "VkQueueFamilyIndex: " << queueFamilyIndex << "\n";
    }

    vvvv::Scoped<VkDevice> device {};
    VkQueue queue {};
    {
        {
            auto feature11 = vvvv::vkStructZero<VkPhysicalDeviceVulkan11Features>([](auto& v) {
                v.shaderDrawParameters = VK_TRUE;
            });
            auto feature13 = vvvv::vkStructZero<VkPhysicalDeviceVulkan13Features>([](auto& v) {
                v.dynamicRendering = VK_TRUE;
            });

            const float queuePriority = 1.0;
            const auto queueCreateInfo = vvvv::vkStructZero<VkDeviceQueueCreateInfo>([&](auto& v) {
                v.queueFamilyIndex = queueFamilyIndex;
                v.queueCount = 1;
                v.pQueuePriorities = &queuePriority;
            });

            auto r = vvvv::CreateVkDevice(physicalDevice)
                         .with([&](auto& opts) noexcept {
                             opts.info.pNext = vvvv::ChainVkBaseOutStructures()(feature11, feature13);
                             opts.info.queueCreateInfoCount = 1;
                             opts.info.pQueueCreateInfos = &queueCreateInfo;
                             opts.allocator = allocator;
                         })();
            if (!r.isOK()) {
                return vvvv::Error::wrap("creating VkDevice", r.error());
            }

            device = std::move(r.ok());
        }

        vkGetDeviceQueue(device.value(), queueFamilyIndex, 0, &queue);

        std::cout << "VkDevice: " << device.value() << "\n";
        std::cout << "VkQueue: " << queue << "\n";
    }

    vvvv::Scoped<VkCommandPool> commandPool {};
    {
        auto r = vvvv::CreateVkCommandPool(device.value())
                     .with([&](auto& opts) noexcept {
                         opts.info.queueFamilyIndex = queueFamilyIndex;
                         opts.info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                         opts.allocator = allocator;
                     })();
        if (!r.isOK()) {
            return vvvv::Error::wrap("creating VkCommandPool", r.error());
        }

        commandPool = std::move(r.ok());
        std::cout << "VkCommandPool: " << commandPool.value() << "\n";
    }

    vvvv::Scoped<vvvv::OnVkDeviceMemory<VkImage>> outputImage {};
    {
        vvvv::Scoped<VkImage> image {};
        {
            auto r = vvvv::CreateVkImage(device.value())
                         .with([&](auto& opts) {
                             opts.info.imageType = VK_IMAGE_TYPE_2D;
                             opts.info.format = VK_FORMAT_R8G8B8A8_UNORM;
                             opts.info.extent = VkExtent3D { .width = 512, .height = 512, .depth = 1 };
                             opts.info.mipLevels = 1;
                             opts.info.arrayLayers = 1;
                             opts.info.samples = VK_SAMPLE_COUNT_1_BIT;
                             opts.info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                             opts.allocator = allocator;
                         })();
            if (!r.isOK()) {
                return vvvv::Error::wrap("creating VkImage for output image", r.error());
            }

            image = std::move(r.ok());
        }

        vvvv::Scoped<VkDeviceMemory> memory {};
        {
            auto reqs = vvvv::vkStructZero<VkMemoryRequirements2>();
            const auto info = vvvv::vkStructZero<VkImageMemoryRequirementsInfo2>([&](auto& v) {
                v.image = image.value();
            });
            vkGetImageMemoryRequirements2(device.value(), &info, &reqs);

            uint32_t typeIndex = 0;
            {
                const auto r = vvvv::FindVkDeviceMemoryTypeIndex(physicalDevice)
                                   .with([&](auto& opts) {
                                       opts.typeBits = reqs.memoryRequirements.memoryTypeBits;
                                   })();
                if (!r.isOK()) {
                    return vvvv::Error::wrap("finding VkDeviceMemory type index for output image", r.error());
                }

                typeIndex = r.ok();
            }

            {
                auto r = vvvv::AllocateVkDeviceMemory(device.value())
                             .with([&](auto& opts) {
                                 opts.info.allocationSize = reqs.memoryRequirements.size;
                                 opts.info.memoryTypeIndex = typeIndex;
                                 opts.allocator = allocator;
                             })();
                if (!r.isOK()) {
                    return vvvv::Error::wrap("allocating VkDeviceMemory for output image", r.error());
                }

                memory = std::move(r.ok());
            }
        }
        {
            const auto infos = std::to_array({
                vvvv::vkStructZero<VkBindImageMemoryInfo>([&](auto& v) {
                    v.image = image.value();
                    v.memory = memory.value();
                }),
            });

            const auto err = vvvv::BindVkImageMemory(device.value())
                                 .with([&](auto& opts) {
                                     opts.infos = infos;
                                 })();
            if (err.has()) {
                return vvvv::Error::wrap("binding VkImage memory for output image", err);
            }
        }

        outputImage = vvvv::Scoped(
            vvvv::OnVkDeviceMemory(image.release(), memory.release()),
            {
                .value = {
                    .device = device.value(),
                    .allocator = allocator,
                },
                .memory = {
                    .device = device.value(),
                    .allocator = allocator,
                },
            }
        );
    }

    vvvv::Renderer renderer {};
    {
        auto r = vvvv::CreateRenderer()
                     .with([&](auto& opts) noexcept {
                         opts.device = device.value();
                         opts.commandPool = commandPool.value();
                         opts.outputImage = outputImage.value().value;
                         opts.allocator = allocator;
                     })();
        if (!r.isOK()) {
            return vvvv::Error::wrap("creating Renderer", r.error());
        }

        renderer = std::move(r.ok());
    }
    for (size_t i = 0; i < 10; ++i) {
        auto err = renderer.render(queue);
        if (err.has()) {
            return vvvv::Error::wrap("rendering", err);
        }
    }

    vvvv::Capturer capturer {};
    {
        auto r = vvvv::CreateCapturer()();
        if (!r.isOK()) {
            return vvvv::Error::wrap("creating Capturer", r.error());
        }

        capturer = r.ok();
    }
    {
        std::vector<std::byte> raw {};
        {
            auto r = capturer.capture(512, 512, 3);
            if (!r.isOK()) {
                return vvvv::Error::wrap("capturing the output image", r.error());
            }

            raw = std::move(r.ok());
        }

        std::vector<std::byte> bmp {};
        {
            auto r = vvvv::BMPEncoder().encode(raw, 512, 512, 3);
            if (!r.isOK()) {
                return vvvv::Error::wrap("encoding the output image to BMP", r.error());
            }

            bmp = std::move(r.ok());
        }

        {
            const auto err = vvvv::FileWriter().write("output.bmp", bmp);
            if (err.has()) {
                return vvvv::Error::wrap("writing output.bmp", err);
            }
        }
    }

    std::cout << "Completed\n";
    return vvvv::Error::none();
}
} // namespace

namespace {
VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugUtilsMessengerCallback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT types,
    const VkDebugUtilsMessengerCallbackDataEXT* const data,
    void* const /* userData */
)
{
    std::string b {};
    b.reserve(512);

    constexpr auto severityLabelSet = std::to_array({
        std::pair { VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "VERBOSE" },
        std::pair { VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, "INFO" },
        std::pair { VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, "WARNING" },
        std::pair { VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "ERROR" },
    });
    for (const auto& [flag, label] : severityLabelSet) {
        if (severity == flag) {
            b += "[";
            b += label;
            b += "]";
            break;
        }
    }

    constexpr auto typeLabelSet = std::to_array({
        std::pair { VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, "GENERAL" },
        std::pair { VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "VALIDATION" },
        std::pair { VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, "PERFORMANCE" },
        std::pair { VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT, "DEVICE_ADDRESS_BINDING" },
    });
    b += "[";
    {
        bool first = true;
        for (const auto& [flag, label] : typeLabelSet) {
            if ((types & flag) != 0U) {
                if (!first) {
                    b += "|";
                    first = false;
                }
                b += label;
            }
        }
    }
    b += "]";

    if (data->pMessage != nullptr) {
        b += " ";
        b.append(data->pMessage);
    }

    b += "\n";

    std::cout << b;
    return VK_FALSE;
}
} // namespace
