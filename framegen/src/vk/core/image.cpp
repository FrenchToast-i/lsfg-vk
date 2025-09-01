#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/device.hpp"
#include "vk/core/image.hpp"
#include "vk/exception.hpp"

#include <optional>
#include <cstdint>
#include <memory>

using namespace VK::Core;

Image::Image(const Device& device, VkExtent2D extent,
        VkFormat format, VkImageUsageFlags usage,
        std::optional<int> importFd, std::optional<int*> exportFd)
        : extent(extent), format(format) {
    // create image
    const VkExternalMemoryImageCreateInfo externalInfo{
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR
    };
    const VkImageCreateInfo desc{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = (importFd.has_value() || exportFd.has_value()) ? &externalInfo : nullptr,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = extent.width,
            .height = extent.height,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    VkImage imageHandle{};
    auto res = vkCreateImage(device.handle(), &desc, nullptr, &imageHandle);
    if (res != VK_SUCCESS || imageHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Failed to create Vulkan image");

    // find memory type
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device.handle(), imageHandle, &memReqs);

    std::optional<uint32_t> memType = device.findMemoryType(memReqs.memoryTypeBits);
    if (!memType.has_value())
        throw VK::vulkan_error(VK_ERROR_UNKNOWN, "Unable to find memory type for buffer");

    // allocate and bind memory
    const VkMemoryDedicatedAllocateInfoKHR dedicatedInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR,
        .image = imageHandle,
    };
    const VkImportMemoryFdInfoKHR importInfo{
        .sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR,
        .pNext = &dedicatedInfo,
        .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR,
        .fd = importFd.value_or(-1)
    };
    const VkExportMemoryAllocateInfo exportInfo{
        .sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
        .pNext = &dedicatedInfo,
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR
    };
    const void* pNextAlloc{};
    if (importFd.has_value())
        pNextAlloc = &importInfo;
    else if (exportFd.has_value())
        pNextAlloc = &exportInfo;
    const VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = pNextAlloc,
        .allocationSize = memReqs.size,
        .memoryTypeIndex = memType.value()
    };
    VkDeviceMemory memoryHandle{};
    res = vkAllocateMemory(device.handle(), &allocInfo, nullptr, &memoryHandle);
    if (res != VK_SUCCESS || memoryHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Failed to allocate memory for Vulkan image");

    res = vkBindImageMemory(device.handle(), imageHandle, memoryHandle, 0);
    if (res != VK_SUCCESS)
        throw VK::vulkan_error(res, "Failed to bind memory to Vulkan image");

    // create image view
    const VkImageViewCreateInfo viewDesc{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = imageHandle,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    VkImageView viewHandle{};
    res = vkCreateImageView(device.handle(), &viewDesc, nullptr, &viewHandle);
    if (res != VK_SUCCESS || viewHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Failed to create image view");

    // obtain the sharing fd
    if (exportFd.has_value()) {
        const VkMemoryGetFdInfoKHR fdInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
            .memory = memoryHandle,
            .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR,
        };
        res = vkGetMemoryFdKHR(device.handle(), &fdInfo, exportFd.value());
        if (res != VK_SUCCESS || *exportFd.value() < 0)
            throw VK::vulkan_error(res, "Failed to obtain sharing fd for Vulkan image");
    }

    // store objects in shared ptr
    this->image = std::shared_ptr<VkImage>(
        new VkImage(imageHandle),
        [dev = device.handle()](VkImage* img) {
            vkDestroyImage(dev, *img, nullptr);
        }
    );
    this->memory = std::shared_ptr<VkDeviceMemory>(
        new VkDeviceMemory(memoryHandle),
        [dev = device.handle()](VkDeviceMemory* mem) {
            vkFreeMemory(dev, *mem, nullptr);
        }
    );
    this->view = std::shared_ptr<VkImageView>(
        new VkImageView(viewHandle),
        [dev = device.handle()](VkImageView* imgView) {
            vkDestroyImageView(dev, *imgView, nullptr);
        }
    );
}
