#pragma once
#include "Mango/Graphics/Image.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkRenderer;
    class VkImage : public Image
    {
    public:
        VkImage(VkRenderer* renderer, U32 width, U32 height, ImageType type, ImageUsage usage, const void* data);
        ~VkImage() override;

        VkImage(const VkImage&) = delete;
        VkImage& operator=(const VkImage&) = delete;

        VkImage(VkImage&&) = delete;
        VkImage& operator=(VkImage&&) = delete;

        vk::Image GetImage() const { return m_Image; }
        vk::ImageView GetView() const { return m_View; }

        void TransitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout from, vk::ImageLayout to,
                              vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 srcAccess,
                              vk::AccessFlags2 dstAccess) const;

        void SetData(const void* data, USize size);

    private:
        void Allocate();

    private:
        VkRenderer* m_Renderer = nullptr;

        vk::Image m_Image;
        vk::DeviceMemory m_Memory;
        vk::ImageView m_View;
        vk::Sampler m_Sampler;

        vk::MemoryPropertyFlags m_MemoryProperties;
        vk::ImageUsageFlags m_Usage;

        vk::Format m_Format;
        vk::ImageAspectFlags m_Aspect;

        vk::Buffer m_StagingBuffer;
        vk::DeviceMemory m_StagingBufferMemory;
        vk::DeviceSize m_AlignedSize;
    };
} // namespace Mango::Driver::Vulkan
