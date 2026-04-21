#include <array>
#include <iostream>
#include <span>
#include <string>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "scoped.h"
#include "vk_debug.h"
#include "vk_instance.h"
#include "vk_pfn.h"

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
        const auto layerNames = std::to_array({
            "VK_LAYER_KHRONOS_validation",
        });
        const auto extensionNames = std::to_array({
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        });

        auto r = vvvv::CreateVkInstance()
                     .with([&](auto& opts) {
                         opts.appInfo.pApplicationName = "vvvv";
                         opts.appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);
                         opts.layerNames = layerNames;
                         opts.extensionNames = extensionNames;
                         opts.allocator = allocator;
                     })
                     .invoke();
        if (!r.isOK()) {
            return vvvv::Error::wrap("creating vkInstance", r.error());
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

    vvvv::Scoped<VkDebugUtilsMessengerEXT> debugMessenger {};
    {
        const auto vkCreate = vvvv::GetVkProcessAddress<VkInstance>(instance.value())
                                  .with([](auto& opts) { opts.name = "vkCreateDebugUtilsMessengerEXT"; })
                                  .invoke<PFN_vkCreateDebugUtilsMessengerEXT>();
        const auto vkDestroy = vvvv::GetVkProcessAddress<VkInstance>(instance.value())
                                   .with([](auto& opts) { opts.name = "vkDestroyDebugUtilsMessengerEXT"; })
                                   .invoke<PFN_vkDestroyDebugUtilsMessengerEXT>();

        auto r = vvvv::CreateVkDebugUtilsMessenger(instance.value())
                     .with([&](auto& opts) {
                         opts.severity = static_cast<VkFlags>(0)
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                         opts.type = static_cast<VkFlags>(0)
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
                         opts.callback = vkDebugUtilsMessengerCallback;
                         opts.allocator = allocator;
                         opts.vkCreateDebugUtilsMessenger = vkCreate;
                         opts.vkDestroyDebugUtilsMessenger = vkDestroy;
                     })
                     .invoke();
        if (!r.isOK()) {
            return vvvv::Error::wrap("creating vkDebugUtilsMessenger", r.error());
        }

        debugMessenger = std::move(r.ok());
        std::cout << "VkDebugUtilsMessenger: " << debugMessenger.value() << "\n";
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

    {
        b += "Objects:\n";
        std::span objs { data->pObjects, data->objectCount };
        for (const auto& obj : objs) {
            b += "- {";

            b += "Type: ";
            b += std::to_string(obj.objectType);

            b += "Handle: ";
            b += std::to_string(obj.objectHandle);

            b += ", Name: ";
            b += obj.pObjectName;

            b += "}\n";
        }
    }

    std::cout << b;

    return VK_FALSE;
}
} // namespace
