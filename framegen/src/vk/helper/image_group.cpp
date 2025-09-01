#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/helper/image_group.hpp"
#include "vk/core/device.hpp"

#include <cstddef>
#include <vector>

using namespace VK::Helper;

ImageGroup::ImageGroup(const Core::Device& device, VkExtent2D extent,
        size_t count, VkFormat format) {
    this->images.reserve(count);
    for (size_t i = 0; i < count; i++)
        this->images.emplace_back(device, extent, format);
}
