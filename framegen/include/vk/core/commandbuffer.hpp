#pragma once

#include "vk/core/descriptorset.hpp"
#include "vk/core/commandpool.hpp"
#include "vk/core/semaphore.hpp"
#include "vk/core/pipeline.hpp"
#include "vk/core/device.hpp"
#include "vk/core/fence.hpp"

#include <vulkan/vulkan_core.h>

#include <optional>
#include <cstdint>
#include <vector>
#include <memory>

namespace VK::Core {

    /// State of the command buffer.
    enum class CommandBufferState {
        /// Command buffer is not initialized or has been destroyed.
        Invalid,
        /// Command buffer has been created.
        Empty,
        /// Command buffer recording has started.
        Recording,
        /// Command buffer recording has ended.
        Full,
        /// Command buffer has been submitted to a queue.
        Submitted
    };

    ///
    /// C++ wrapper class for a Vulkan command buffer.
    ///
    /// This class manages the lifetime of a Vulkan command buffer.
    ///
    class CommandBuffer {
    public:
        CommandBuffer() noexcept = default;

        ///
        /// Create the command buffer.
        ///
        /// @param device Vulkan device
        /// @param pool Vulkan command pool
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        CommandBuffer(const Device& device, const CommandPool& pool);

        ///
        /// Begin recording commands in the command buffer.
        ///
        /// @throws std::logic_error if the command buffer is not in Empty state
        /// @throws VK::vulkan_error if beginning the command buffer fails.
        ///
        void begin();

        ///
        /// Bind a compute pipeline to the command buffer.
        ///
        /// @param pipeline Vulkan compute pipeline
        ///
        /// @throws std::logic_error if the command buffer is not in Recording state
        ///
        void bindPipeline(const Pipeline& pipeline) const;

        ///
        /// Bind a descriptor set to the command buffer.
        ///
        /// @param pipeline Vulkan compute pipeline
        /// @param set Vulkan descriptor set
        ///
        /// @throws std::logic_error if the command buffer is not in Recording state
        ///
        void bindDescriptorSet(const Pipeline& pipeline, const DescriptorSet& set) const;

        ///
        /// Insert memory barriers transitioning images into the general layout.
        ///
        /// @param images Images to transition to general layout
        ///
        /// @throws std::logic_error if the command buffer is not in Recording state
        ///
        void insertBarrier(const std::vector<VkImage>& images) const;

        ///
        /// Insert memory barriers for images in the command buffer.
        ///
        /// @param readableImages Images that will be transitioned from rw to read-only
        /// @param writableImages Images that will be transitioned from read-only to rw
        ///
        /// @throws std::logic_error if the command buffer is not in Recording state
        ///
        void insertBarrier(
            const std::vector<std::optional<Core::Image>>& readableImages,
            const std::vector<Core::Image>& writableImages) const;

        ///
        /// Copy a buffer to an image.
        ///
        /// @param buffer Vulkan buffer
        /// @param image Vulkan image
        ///
        /// @throws std::logic_error if the command buffer is not in Recording state
        ///
        void copyBufferToImage(const Buffer& buffer, const Image& image) const;

        ///
        /// Clear an image to a color.
        ///
        /// @param image Vulkan image
        /// @param white If true, clear to white; otherwise, clear to black
        ///
        /// @throws std::logic_error if the command buffer is not in Recording state
        ///
        void clearImage(const Image& image, bool white) const;

        ///
        /// Dispatch a compute command.
        ///
        /// @param x Number of groups in the X dimension
        /// @param y Number of groups in the Y dimension
        /// @param z Number of groups in the Z dimension
        ///
        /// @throws std::logic_error if the command buffer is not in Recording state
        ///
        void dispatch(uint32_t x, uint32_t y, uint32_t z) const;

        ///
        /// End recording commands in the command buffer.
        ///
        /// @throws std::logic_error if the command buffer is not in Recording state
        /// @throws VK::vulkan_error if ending the command buffer fails.
        ///
        void end();

        ///
        /// Submit the command buffer to a queue.
        ///
        /// @param device Vulkan device
        /// @param fence Optional fence to signal after execution
        /// @param wait Semaphores to wait on
        /// @param signal Semaphores to signal after execution
        /// @param waitTimelines Timeline semaphores to wait on
        /// @param signalTimelines Timeline semaphores to signal after execution
        ///
        /// @throws std::logic_error if the command buffer is not in Full state.
        /// @throws VK::vulkan_error if submission fails.
        ///
        void submit(const Device& device, std::optional<Fence> fence,
            const std::vector<Semaphore>& wait = {},
            const std::vector<Semaphore>& signal = {},
            const std::vector<std::pair<Semaphore, uint64_t>>& waitTimelines = {},
            const std::vector<std::pair<Semaphore, uint64_t>>& signalTimelines = {});

        /// Get the state of the command buffer.
        [[nodiscard]] auto getState() const { return *this->state; }
        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->commandBuffer; }
    private:
        std::shared_ptr<CommandBufferState> state;
        std::shared_ptr<VkCommandBuffer> commandBuffer;
    };

}
