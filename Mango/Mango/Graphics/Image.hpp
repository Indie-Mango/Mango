#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango {
    class Renderer;

    class Image
    {
    public:
        Image(U32 width, U32 height, vk::Format format, vk::ImageAspectFlags aspect, vk::ImageUsageFlags usage, vk::Device d,
              vk::PhysicalDevice pd, vk::MemoryPropertyFlags memoryProperties, const void* data, Renderer* renderer);
        ~Image();

        void TransitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout from, vk::ImageLayout to,
                              vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 srcAccess,
                              vk::AccessFlags2 dstAccess) const;

        void Write(const void* data, USize size, Renderer* renderer);

        U32 GetWidth() const { return m_Width; }
        U32 GetHeight() const { return m_Height; }

        vk::Image GetImage() const { return m_Image; }
        vk::ImageView GetView() const { return m_View; }

    private:
        void Allocate();

    private:
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_Device;

        vk::Image m_Image;
        vk::DeviceMemory m_Memory;
        vk::ImageView m_View;
        vk::Sampler m_Sampler;

        vk::MemoryPropertyFlags m_MemoryProperties;
        vk::ImageUsageFlags m_Usage;

        vk::Format m_Format = vk::Format::eUndefined;
        vk::ImageAspectFlags m_Aspect = vk::ImageAspectFlagBits::eNone;

        vk::Buffer m_StagingBuffer;
        vk::DeviceMemory m_StagingBufferMemory;
        vk::DeviceSize m_AlignedSize;

        U32 m_Width = 0, m_Height = 0;
    };
} // namespace Mango
