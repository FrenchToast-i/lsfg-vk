#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/commandbuffer.hpp"
#include "vk/core/descriptorset.hpp"
#include "vk/core/commandpool.hpp"
#include "vk/core/semaphore.hpp"
#include "vk/core/pipeline.hpp"
#include "vk/core/buffer.hpp"
#include "vk/core/device.hpp"
#include "vk/core/fence.hpp"
#include "vk/core/image.hpp"
#include "vk/exception.hpp"

#include <stdexcept>
#include <optional>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <memory>
#include <vector>

using namespace VK::Core;

CommandBuffer::CommandBuffer(const Device& device, const CommandPool& pool) {
    // create command buffer
    const VkCommandBufferAllocateInfo desc{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool.handle(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    VkCommandBuffer commandBufferHandle{};
    auto res = vkAllocateCommandBuffers(device.handle(), &desc, &commandBufferHandle);
    if (res != VK_SUCCESS || commandBufferHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Unable to allocate command buffer");

    // store command buffer in shared ptr
    this->state = std::make_shared<CommandBufferState>(CommandBufferState::Empty);
    this->commandBuffer = std::shared_ptr<VkCommandBuffer>(
        new VkCommandBuffer(commandBufferHandle),
        [dev = device.handle(), pool = pool.handle()](VkCommandBuffer* cmdBuffer) {
            vkFreeCommandBuffers(dev, pool, 1, cmdBuffer);
        }
    );
}

void CommandBuffer::begin() {
    if (*this->state != CommandBufferState::Empty)
        throw std::logic_error("Command buffer is not in Empty state");

    const VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    auto res = vkBeginCommandBuffer(*this->commandBuffer, &beginInfo);
    if (res != VK_SUCCESS)
        throw VK::vulkan_error(res, "Unable to begin command buffer");

    *this->state = CommandBufferState::Recording;
}

void CommandBuffer::bindPipeline(const Pipeline& pipeline) const {
    if (*this->state != CommandBufferState::Recording)
        throw std::logic_error("Command buffer is not in Recording state");

    vkCmdBindPipeline(*this->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.handle());
}

void CommandBuffer::bindDescriptorSet(const Pipeline& pipeline, const DescriptorSet& set) const {
    if (*this->state != CommandBufferState::Recording)
        throw std::logic_error("Command buffer is not in Recording state");

    VkDescriptorSet descriptorSetHandle = set.handle();
    vkCmdBindDescriptorSets(*this->commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.getLayout(),
        0, 1, &descriptorSetHandle, 0, nullptr);
}

void CommandBuffer::insertBarrier(
        const std::vector<VkImage>& images) const {
    if (*this->state != CommandBufferState::Recording)
        throw std::logic_error("Command buffer is not in Recording state");

    std::vector<VkImageMemoryBarrier2> barriers(images.size());
    for (size_t i = 0; i < images.size(); i++) {
        barriers[i] = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
            .image = images[i],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1
            }
        };
    }

    const VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
        .pImageMemoryBarriers = barriers.data()
    };
    vkCmdPipelineBarrier2(*this->commandBuffer, &dependencyInfo);
}

void CommandBuffer::insertBarrier(
        const std::vector<VkImage>& readableImages,
        const std::vector<VkImage>& writableImages) const {
    if (*this->state != CommandBufferState::Recording)
        throw std::logic_error("Command buffer is not in Recording state");

    // create barriers
    const VkImageMemoryBarrier2 dummyBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    const size_t totalImages =
        readableImages.size() + writableImages.size();
    std::vector<VkImageMemoryBarrier2> barriers(totalImages);

    for (const auto& image : readableImages) {
        VkImageMemoryBarrier2& barrier = barriers.emplace_back(dummyBarrier);
        barrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
        barrier.image = image;
    }

    for (const auto& image : writableImages) {
        VkImageMemoryBarrier2& barrier = barriers.emplace_back(dummyBarrier);
        barrier.srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
        barrier.image = image;
    }

    // insert barriers
    const VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
        .pImageMemoryBarriers = barriers.data()
    };
    vkCmdPipelineBarrier2(*this->commandBuffer, &dependencyInfo);
}

void CommandBuffer::copyBufferToImage(const Buffer& buffer, const Image& image) const {
    if (*this->state != CommandBufferState::Recording)
        throw std::logic_error("Command buffer is not in Recording state");

    const auto extent = image.getExtent();
    const VkBufferImageCopy region{
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .layerCount = 1
        },
        .imageExtent = { extent.width, extent.height, 1 }
    };
    vkCmdCopyBufferToImage(
        *this->commandBuffer,
        buffer.handle(), image.handle(),
        VK_IMAGE_LAYOUT_GENERAL, 1, &region
    );
}

void CommandBuffer::clearImage(const Image& image, bool white) const {
    if (*this->state != CommandBufferState::Recording)
        throw std::logic_error("Command buffer is not in Recording state");

    const float clearValue = white ? 1.0F : 0.0F;
    const VkClearColorValue clearColor = {{ clearValue, clearValue, clearValue, clearValue }};
    const VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1
    };
    vkCmdClearColorImage(*this->commandBuffer,
        image.handle(), VK_IMAGE_LAYOUT_GENERAL,
        &clearColor,
        1, &subresourceRange);
}

void CommandBuffer::dispatch(uint32_t x, uint32_t y, uint32_t z) const {
    if (*this->state != CommandBufferState::Recording)
        throw std::logic_error("Command buffer is not in Recording state");

    vkCmdDispatch(*this->commandBuffer, x, y, z);
}

void CommandBuffer::end() {
    if (*this->state != CommandBufferState::Recording)
        throw std::logic_error("Command buffer is not in Recording state");

    auto res = vkEndCommandBuffer(*this->commandBuffer);
    if (res != VK_SUCCESS)
        throw VK::vulkan_error(res, "Unable to end command buffer");

    *this->state = CommandBufferState::Full;
}

void CommandBuffer::submit(const Device& device, std::optional<Fence> fence,
        const std::vector<Semaphore>& wait,
        const std::vector<Semaphore>& signal,
        const std::vector<std::pair<Semaphore, uint64_t>>& waitTimelines,
        const std::vector<std::pair<Semaphore, uint64_t>>& signalTimelines) {
    if (*this->state != CommandBufferState::Full)
        throw std::logic_error("Command buffer is not in Full state");

    // create wait semaphores and values
    std::vector<VkSemaphore> waitSemaphores(waitTimelines.size() + wait.size());
    std::vector<uint64_t> waitSemaphoreValues(waitTimelines.size() + wait.size());
    for (const auto& entry : waitTimelines) {
        waitSemaphores.push_back(entry.first.handle());
        waitSemaphoreValues.push_back(entry.second);
    }
    for (const auto& semaphore : wait)
        waitSemaphores.push_back(semaphore.handle());

    // create signal semaphores and values
    std::vector<VkSemaphore> signalSemaphores(signalTimelines.size() + signal.size());
    std::vector<uint64_t> signalSemaphoreValues(signalTimelines.size() + signal.size());
    for (const auto& entry : signalTimelines) {
        signalSemaphores.push_back(entry.first.handle());
        signalSemaphoreValues.push_back(entry.second);
    }
    for (const auto& semaphore : signal)
        signalSemaphores.push_back(semaphore.handle());

    // submit command buffer
    const VkTimelineSemaphoreSubmitInfo timelineInfo{
        .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
        .waitSemaphoreValueCount = static_cast<uint32_t>(waitSemaphoreValues.size()),
        .pWaitSemaphoreValues = waitSemaphoreValues.data(),
        .signalSemaphoreValueCount = static_cast<uint32_t>(signalSemaphoreValues.size()),
        .pSignalSemaphoreValues = signalSemaphoreValues.data()
    };
    const std::vector<VkPipelineStageFlags> waitStages(waitSemaphores.size(),
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
    const VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = waitSemaphores.empty() && signalSemaphores.empty()
            ? nullptr : &timelineInfo,
        .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = 1,
        .pCommandBuffers = &(*this->commandBuffer),
        .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
        .pSignalSemaphores = signalSemaphores.data()
    };
    auto res = vkQueueSubmit(device.getComputeQueue(),
        1, &submitInfo,
        fence ? fence->handle() : VK_NULL_HANDLE);
    if (res != VK_SUCCESS)
        throw VK::vulkan_error(res, "Unable to submit command buffer");

    *this->state = CommandBufferState::Submitted;
}
