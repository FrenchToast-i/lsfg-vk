#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/helper/mipmapped_image.hpp"
#include "vk/core/device.hpp"

#include <cstddef>
#include <vector>

using namespace VK::Helper;

MipmappedImage::MipmappedImage(const Core::Device& device, VkExtent2D extent,
        size_t levels, VkFormat format) {
    this->images.reserve(levels);
    for (size_t i = 0; i < levels; i++)
        this->images.emplace_back(
            device,
            VkExtent2D { extent.width >> i, extent.height >> i },
            format
        );
}
