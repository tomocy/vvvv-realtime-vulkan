#include <iostream>
#include <vulkan/vulkan_core.h>

#include "error.h"
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

    VkInstance instance = VK_NULL_HANDLE;
    {
        auto [v, err] = vvvv::CreateVkInstance()
                            .with([&](auto& opts) {
                                opts.appInfo.pApplicationName = "vvvv";
                                opts.appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);
                                opts.allocator = allocator;
                            })
                            .invoke();
        if (err.has()) {
            return vvvv::Error::wrap("creating vkInstance", err);
        }

        instance = v;
        std::cout << "VkInstance: " << instance << "\n";
    }

    vkDestroyInstance(instance, allocator);

    std::cout << "Completed\n";
    return vvvv::Error::none();
}
} // namespace
