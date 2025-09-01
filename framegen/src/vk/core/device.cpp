#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/instance.hpp"
#include "vk/core/device.hpp"
#include "vk/exception.hpp"

#include <iostream>
#include <optional>
#include <cstdint>
#include <bitset>
#include <memory>
#include <vector>
#include <array>

using namespace VK::Core;

const std::array<const char*, 3> requiredExtensions = {
    "VK_KHR_external_memory_fd",
    "VK_KHR_external_semaphore_fd",
    "VK_EXT_robustness2"
};

namespace {
    /// Find a physical device by its UUID.
    std::optional<VkPhysicalDevice> findDeviceByUUID(const Instance& instance, uint64_t uuid) {
        uint32_t deviceCount{};
        auto res = vkEnumeratePhysicalDevices(instance.handle(), &deviceCount, nullptr);
        if (res != VK_SUCCESS || deviceCount == 0)
            throw VK::vulkan_error(res, "Failed to enumerate physical devices");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        res = vkEnumeratePhysicalDevices(instance.handle(), &deviceCount, devices.data());
        if (res != VK_SUCCESS)
            throw VK::vulkan_error(res, "Failed to get physical devices");

        for (const auto& device : devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            const uint64_t gpuid =
                static_cast<uint64_t>(properties.vendorID) << 32 | properties.deviceID;
            if (uuid == gpuid || uuid == 0x1463ABAC)
                return device;
        }
        return std::nullopt;
    }

    /// Find the compute queue family index.
    std::optional<uint32_t> findComputeQueueFamily(VkPhysicalDevice device) {
        uint32_t familyCount{};
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());

        for (uint32_t i = 0; i < familyCount; ++i) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                return i;
        }
        return std::nullopt;
    }

    /// Query the physical device for FP16 support.
    bool checkFP16Support(VkPhysicalDevice device) {
        VkPhysicalDeviceVulkan12Features features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
        };
        VkPhysicalDeviceFeatures2 features2{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &features
        };
        vkGetPhysicalDeviceFeatures2(device, &features2);
        return features.shaderFloat16;
    }
}

Device::Device(const Instance& instance, uint64_t uuid, bool enforceFP32) {
    // get device by uuid
    std::optional<VkPhysicalDevice> physicalDevice = findDeviceByUUID(instance, uuid);
    if (!physicalDevice)
        throw VK::vulkan_error(VK_ERROR_INITIALIZATION_FAILED,
            "Could not find physical device with UUID");

    // find queue family indices
    std::optional<uint32_t> computeFamilyIdx = findComputeQueueFamily(*physicalDevice);
    if (!computeFamilyIdx)
        throw VK::vulkan_error(VK_ERROR_INITIALIZATION_FAILED, "No compute queue family found");

    // print fp16 debug info
    const bool fp16 = !enforceFP32 && checkFP16Support(*physicalDevice);
    if (fp16)
        std::cerr << "lsfg-vk: Using FP16 acceleration" << '\n';
    else if (!enforceFP32)
        std::cerr << "lsfg-vk: FP16 acceleration not supported, using FP32" << '\n';

    // create logical device
    const float queuePriority{1.0F}; // highest priority
    VkPhysicalDeviceRobustness2FeaturesEXT robustness2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT,
        .nullDescriptor = VK_TRUE
    };
    VkPhysicalDeviceVulkan13Features features13{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &robustness2,
        .synchronization2 = VK_TRUE
    };
    const VkPhysicalDeviceVulkan12Features features12{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &features13,
        .shaderFloat16 = fp16,
        .timelineSemaphore = VK_TRUE,
        .vulkanMemoryModel = VK_TRUE
    };
    const VkDeviceQueueCreateInfo computeQueueDesc{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = *computeFamilyIdx,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };
    const VkDeviceCreateInfo deviceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features12,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &computeQueueDesc,
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };
    VkDevice deviceHandle{};
    auto res = vkCreateDevice(*physicalDevice, &deviceCreateInfo, nullptr, &deviceHandle);
    if (res != VK_SUCCESS | deviceHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Failed to create logical device");

    // get queue
    volkLoadDevice(deviceHandle);

    VkQueue queueHandle{};
    vkGetDeviceQueue(deviceHandle, *computeFamilyIdx, 0, &queueHandle);
    if (!queueHandle)
        throw VK::vulkan_error(VK_ERROR_INITIALIZATION_FAILED, "Failed to get compute queue");

    // store in shared ptr
    this->fp16 = fp16;
    this->computeFamilyIdx = *computeFamilyIdx;
    this->computeQueue = queueHandle;
    this->physicalDevice = *physicalDevice;
    this->device = std::shared_ptr<VkDevice>(
        new VkDevice(deviceHandle),
        [](VkDevice* device) {
            vkDestroyDevice(*device, nullptr);
        }
    );
}

std::optional<uint32_t> Device::findMemoryType(std::bitset<32> validTypes, bool hostVisible) const {
    const VkMemoryPropertyFlags desiredProps = hostVisible ?
        (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) :
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &memProps);

    std::array<VkMemoryType, 32> memTypes = std::to_array(memProps.memoryTypes);
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
        if (validTypes.test(i) && (memTypes.at(i).propertyFlags & desiredProps) == desiredProps)
            return i;

    return std::nullopt;
}
