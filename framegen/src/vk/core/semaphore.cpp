#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/semaphore.hpp"
#include "vk/core/device.hpp"
#include "vk/exception.hpp"

#include <optional>
#include <memory>

using namespace VK::Core;

Semaphore::Semaphore(const Device& device, std::optional<int> fd) {
    // create semaphore
    const VkExportSemaphoreCreateInfo exportInfo{
        .sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO,
        .handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT
    };
    const VkSemaphoreCreateInfo desc{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = fd.has_value() ? &exportInfo : nullptr
    };
    VkSemaphore semaphoreHandle{};
    auto res = vkCreateSemaphore(device.handle(), &desc, nullptr, &semaphoreHandle);
    if (res != VK_SUCCESS || semaphoreHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Unable to create semaphore");

    if (fd.has_value()) {
        // import semaphore from fd
        auto vkImportSemaphoreFdKHR = reinterpret_cast<PFN_vkImportSemaphoreFdKHR>(
            vkGetDeviceProcAddr(device.handle(), "vkImportSemaphoreFdKHR"));

        const VkImportSemaphoreFdInfoKHR importInfo{
            .sType = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR,
            .semaphore = semaphoreHandle,
            .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT,
            .fd = *fd // closes the fd
        };
        res = vkImportSemaphoreFdKHR(device.handle(), &importInfo);
        if (res != VK_SUCCESS)
            throw VK::vulkan_error(res, "Unable to import semaphore from fd");
    }

    // store semaphore in shared ptr
    this->semaphore = std::shared_ptr<VkSemaphore>(
        new VkSemaphore(semaphoreHandle),
        [dev = device.handle()](VkSemaphore* semaphoreHandle) {
            vkDestroySemaphore(dev, *semaphoreHandle, nullptr);
        }
    );
}
