#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/instance.hpp"
#include "vk/exception.hpp"

#include <cstdint>
#include <memory>
#include <vector>

using namespace VK::Core;

const std::vector<const char*> requiredExtensions = {
    // empty, for now :3
};

Instance::Instance() {
    volkInitialize();

    // create Vulkan instance
    const VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "lsfg-vk-base",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "lsfg-vk-base",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_3
    };
    const VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };
    VkInstance instanceHandle{};
    auto res = vkCreateInstance(&createInfo, nullptr, &instanceHandle);
    if (res != VK_SUCCESS)
        throw VK::vulkan_error(res, "Failed to create Vulkan instance");

    volkLoadInstance(instanceHandle);

    // store in shared ptr
    this->instance = std::shared_ptr<VkInstance>(
        new VkInstance(instanceHandle),
        [](VkInstance* instance) {
            vkDestroyInstance(*instance, nullptr);
        }
    );
}
