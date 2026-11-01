#pragma once
#include "Mango/Graphics/Image.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkRenderer;
    class VkImage : public Image
    {
    public:
        VkImage(VkRenderer* renderer, U32 width, U32 height, ImageFormat format, ImageType type, ImageUsage usage, const void* data);
        ~VkImage() override;

        VkImage(const VkImage&) = delete;
        VkImage& operator=(const VkImage&) = delete;

        VkImage(VkImage&&) = delete;
        VkImage& operator=(VkImage&&) = delete;

        void Resize(U32 width, U32 height) override;
        void SetData(const void* data) override;

        vk::Image GetImage() const { return m_Image; }
        vk::ImageView GetView() const { return m_View; }
        vk::Sampler GetSampler() const { return m_Sampler; }
        vk::Format GetVkFormat() const { return m_VkFormat; }

        void TransitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout from, vk::ImageLayout to,
                              vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 srcAccess,
                              vk::AccessFlags2 dstAccess) const;

    private:
        void Allocate();
        void Free();

        void CreateSampler();

        USize GetDataSize() const;
        bool IsFormatSupported() const;

    private:
        VkRenderer* m_Renderer = nullptr;

        vk::Image m_Image{};
        vk::DeviceMemory m_Memory{};
        vk::ImageView m_View{};
        vk::Sampler m_Sampler{};

        ImageUsage m_MangoUsage;
        vk::ImageUsageFlags m_Usage{};
        vk::Format m_VkFormat = vk::Format::eUndefined;
        vk::ImageAspectFlags m_Aspect{};
    };
} // namespace Mango::Driver::Vulkan
