#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/buffer.hpp"
#include "vk/core/device.hpp"
#include "vk/exception.hpp"

#include <algorithm>
#include <optional>
#include <cstddef>
#include <cstdint>
#include <memory>

using namespace VK::Core;

Buffer::Buffer(const Device& device, const void* data, size_t size, VkBufferUsageFlags usage) {
    // create buffer
    const VkBufferCreateInfo desc{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    VkBuffer bufferHandle{};
    auto res = vkCreateBuffer(device.handle(), &desc, nullptr, &bufferHandle);
    if (res != VK_SUCCESS || bufferHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Failed to create Vulkan buffer");

    // find memory type
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device.handle(), bufferHandle, &memReqs);

    std::optional<uint32_t> memType = device.findMemoryType(memReqs.memoryTypeBits, true);
    if (!memType.has_value())
        throw VK::vulkan_error(VK_ERROR_UNKNOWN, "Unable to find memory type for buffer");

    // allocate and bind memory
    const VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memReqs.size,
        .memoryTypeIndex = memType.value()
    };
    VkDeviceMemory memoryHandle{};
    res = vkAllocateMemory(device.handle(), &allocInfo, nullptr, &memoryHandle);
    if (res != VK_SUCCESS || memoryHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Failed to allocate memory for Vulkan buffer");

    res = vkBindBufferMemory(device.handle(), bufferHandle, memoryHandle, 0);
    if (res != VK_SUCCESS)
        throw VK::vulkan_error(res, "Failed to bind memory to Vulkan buffer");

    // upload data to buffer
    uint8_t* buf{};
    res = vkMapMemory(device.handle(), memoryHandle, 0, size, 0, reinterpret_cast<void**>(&buf));
    if (res != VK_SUCCESS || buf == nullptr)
        throw VK::vulkan_error(res, "Failed to map memory for Vulkan buffer");
    std::copy_n(reinterpret_cast<const uint8_t*>(data), size, buf);
    vkUnmapMemory(device.handle(), memoryHandle);

    // store buffer and memory in shared ptr
    this->size = size;
    this->buffer = std::shared_ptr<VkBuffer>(
        new VkBuffer(bufferHandle),
        [dev = device.handle()](VkBuffer* img) {
            vkDestroyBuffer(dev, *img, nullptr);
        }
    );
    this->memory = std::shared_ptr<VkDeviceMemory>(
        new VkDeviceMemory(memoryHandle),
        [dev = device.handle()](VkDeviceMemory* mem) {
            vkFreeMemory(dev, *mem, nullptr);
        }
    );
}
