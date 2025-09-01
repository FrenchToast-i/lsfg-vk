#pragma once

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

        // TODO: Method for binding a descriptor set.
        //   TODO: Rework abstraction for descriptor sets.
        // TODO: Method for inserting a pipeline barrier.
        //   TODO: Rework abstraction for barriers.
        // TODO: Method for copying a buffer to an image
        // TODO: Method for clearing an image to a color

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

        // FIXME: Submit logic is kind of janky.

        ///
        /// Submit the command buffer to a queue.
        ///
        /// @param queue Vulkan queue to submit to
        /// @param fence Optional fence to signal when the command buffer has finished executing
        /// @param waitSemaphores Semaphores to wait on before executing the command buffer
        /// @param waitSemaphoreValues Values for the semaphores to wait on
        /// @param signalSemaphores Semaphores to signal after executing the command buffer
        /// @param signalSemaphoreValues Values for the semaphores to signal
        ///
        /// @throws std::logic_error if the command buffer is not in Full state.
        /// @throws VK::vulkan_error if submission fails.
        ///
        void submit(VkQueue queue, std::optional<Fence> fence,
            const std::vector<Semaphore>& waitSemaphores = {},
            std::optional<std::vector<uint64_t>> waitSemaphoreValues = std::nullopt,
            const std::vector<Semaphore>& signalSemaphores = {},
            std::optional<std::vector<uint64_t>> signalSemaphoreValues = std::nullopt);

        /// Get the state of the command buffer.
        [[nodiscard]] auto getState() const { return *this->state; }
        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->commandBuffer; }

        /// Trivially copyable, moveable and destructible
        CommandBuffer(const CommandBuffer&) noexcept = default;
        CommandBuffer& operator=(const CommandBuffer&) noexcept = default;
        CommandBuffer(CommandBuffer&&) noexcept = default;
        CommandBuffer& operator=(CommandBuffer&&) noexcept = default;
        ~CommandBuffer() = default;
    private:
        std::shared_ptr<CommandBufferState> state;
        std::shared_ptr<VkCommandBuffer> commandBuffer;
    };

}
