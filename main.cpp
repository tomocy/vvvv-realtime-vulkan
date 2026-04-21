#include <array>
#include <iostream>
#include <vulkan/vulkan_core.h>

#include "error.h"
#include "scoped.h"
#include "vk_instance.h"

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
vvvv::Error run()
{
    VkAllocationCallbacks* allocator = nullptr;

    vvvv::Scoped<VkInstance> instance {};
    {
        const auto layerNames = std::to_array({
            "VK_LAYER_KHRONOS_validation",
        });

        auto r = vvvv::CreateVkInstance()
                     .with([&](auto& opts) {
                         opts.appInfo.pApplicationName = "vvvv";
                         opts.appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);
                         opts.layerNames = layerNames;
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
    }

    std::cout << "Completed\n";
    return vvvv::Error::none();
}
} // namespace
