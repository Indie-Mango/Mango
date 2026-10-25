#include "VkImage.hpp"

#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Driver/Vulkan/VkUtils.hpp"
#include "Mango/Core/Core.hpp"

namespace Mango::Driver::Vulkan {
    namespace {
        namespace Utils {

            vk::ImageAspectFlags MangoImageTypeToVulkanImageAspect(ImageType type)
            {
                switch (type)
                {
                    case ImageType::Color:
                        return vk::ImageAspectFlagBits::eColor;
                }

                return vk::ImageAspectFlagBits::eNone;
            }

            vk::ImageUsageFlags MangoImageUsageToVulkanImageUsage(ImageUsage usage)
            {
                switch (usage)
                {
                    case ImageUsage::ColorAttachment:
                        return vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment;
                }

                return vk::ImageUsageFlagBits::eSampled;
            }
        } // namespace Utils
    } // namespace

    VkImage::VkImage(VkRenderer* renderer, U32 width, U32 height, ImageType type, ImageUsage usage, const void* data) :
        Image(width, height), m_Renderer(renderer), m_Usage(Utils::MangoImageUsageToVulkanImageUsage(usage)),
        m_Format(renderer->GetSurfaceFormat().format), m_Aspect(Utils::MangoImageTypeToVulkanImageAspect(type))
    {
        Allocate();
        if (data)
            SetData(data, static_cast<USize>(m_Width) * m_Height * 4);
    }

    VkImage::~VkImage()
    {
        vk::Device d = m_Renderer->GetDevice();

        d.destroyBuffer(m_StagingBuffer);
        d.free(m_StagingBufferMemory);

        d.destroySampler(m_Sampler);
        d.destroyImageView(m_View);
        d.destroyImage(m_Image);
        d.free(m_Memory);
    }

    void VkImage::TransitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout from, vk::ImageLayout to,
                                   vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 srcAccess,
                                   vk::AccessFlags2 dstAccess) const
    {
        VkUtils::TransitionImageLayout(commandBuffer, from, to, srcStage, dstStage, srcAccess, dstAccess, m_Aspect, m_Image);
    }

    // TODO: Store image size internally or calculate based on format width and height
    void VkImage::SetData(const void* data, USize size)
    {
        vk::PhysicalDevice pd = m_Renderer->GetPhysicalDevice();
        vk::Device d = m_Renderer->GetDevice();

        vk::CommandBuffer transferCommandBuffer = m_Renderer->GetTransferCommandBufferRaw();

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        vk::Result beginRes = transferCommandBuffer.begin(beginInfo);
        ASSERT(beginRes == vk::Result::eSuccess, "Failed to begin transfer command buffer recording");

        if (!m_StagingBuffer)
        {
            U32 transferQueueFamilyIdx = m_Renderer->GetTransferQueueFamilyIdx();

            vk::BufferCreateInfo stagingBufferInfo;
            stagingBufferInfo.size = size;
            stagingBufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            stagingBufferInfo.sharingMode = vk::SharingMode::eExclusive;
            stagingBufferInfo.queueFamilyIndexCount = 1;
            stagingBufferInfo.pQueueFamilyIndices = &transferQueueFamilyIdx;

            vk::ResultValue<vk::Buffer> stagingBuffer = d.createBuffer(stagingBufferInfo);
            ASSERT(stagingBuffer.result == vk::Result::eSuccess, "Failed to create staging buffer");
            m_StagingBuffer = stagingBuffer.value;

            vk::MemoryRequirements memoryRequirements;
            d.getBufferMemoryRequirements(m_StagingBuffer, &memoryRequirements);
            m_AlignedSize = memoryRequirements.size;

            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memoryRequirements.size;
            allocInfo.memoryTypeIndex = VkUtils::FindMemoryTypeIdx(
                    memoryRequirements.memoryTypeBits,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pd);

            vk::ResultValue<vk::DeviceMemory> memory = d.allocateMemory(allocInfo);
            ASSERT(memory.result == vk::Result::eSuccess, "Failed to allocate memory for staging buffer");
            m_StagingBufferMemory = memory.value;

            vk::Result memoryBindRes = d.bindBufferMemory(m_StagingBuffer, m_StagingBufferMemory, 0);
            ASSERT(memoryBindRes == vk::Result::eSuccess, "Failed to bind staging buffer memory to buffer");
        }

        vk::ResultValue<void*> mapRes = d.mapMemory(m_StagingBufferMemory, 0, m_AlignedSize);
        ASSERT(mapRes.result == vk::Result::eSuccess, "Failed to map memory of staging buffer");
        void* mapped = mapRes.value;

        memcpy(mapped, data, size);

        d.unmapMemory(m_StagingBufferMemory);

        {
            vk::ImageMemoryBarrier copyBarrier;
            copyBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
            copyBarrier.oldLayout = vk::ImageLayout::eUndefined;
            copyBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
            copyBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copyBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copyBarrier.image = m_Image;
            copyBarrier.subresourceRange.aspectMask = m_Aspect;
            copyBarrier.subresourceRange.layerCount = 1;
            copyBarrier.subresourceRange.levelCount = 1;

            transferCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, {}, 0,
                                                  nullptr, 0, nullptr, 1, &copyBarrier);
        }

        {
            vk::BufferImageCopy region;
            region.imageSubresource.aspectMask = m_Aspect;
            region.imageSubresource.layerCount = 1;
            region.imageExtent = vk::Extent3D{m_Width, m_Height, 1};
            transferCommandBuffer.copyBufferToImage(m_StagingBuffer, m_Image, vk::ImageLayout::eTransferDstOptimal, region);
        }

        {
            vk::ImageMemoryBarrier useBarrier;
            useBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            useBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            useBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            useBarrier.newLayout = vk::ImageLayout::eUndefined;
            useBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            useBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            useBarrier.image = m_Image;
            useBarrier.subresourceRange.aspectMask = m_Aspect;
            useBarrier.subresourceRange.layerCount = 1;
            useBarrier.subresourceRange.levelCount = 1;

            transferCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {},
                                                  {}, {}, useBarrier);
        }

        vk::Result endRes = transferCommandBuffer.end();
        ASSERT(endRes == vk::Result::eSuccess, "Failed to end transfer command buffer recording");

        vk::SubmitInfo submission;
        submission.commandBufferCount = 1;
        submission.pCommandBuffers = &transferCommandBuffer;
        m_Renderer->SubmitTransfer();
    }

    void VkImage::Allocate()
    {
        vk::PhysicalDevice pd = m_Renderer->GetPhysicalDevice();
        vk::Device d = m_Renderer->GetDevice();

        vk::ImageCreateInfo imageInfo;
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.format = m_Format;
        imageInfo.extent = vk::Extent3D{m_Width, m_Height, 1};
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = vk::SampleCountFlagBits::e1;
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.usage = m_Usage;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ResultValue<vk::Image> imageRes = d.createImage(imageInfo);
        ASSERT(imageRes.result == vk::Result::eSuccess, "Failed to create vulkan image");
        m_Image = imageRes.value;

        vk::MemoryRequirements memoryRequirements = d.getImageMemoryRequirements(m_Image);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits, m_MemoryProperties, pd);
        vk::ResultValue<vk::DeviceMemory> memory = d.allocateMemory(allocInfo);
        ASSERT(memory.result == vk::Result::eSuccess, "Failed to allocate memory for image");
        m_Memory = memory.value;

        vk::Result memoryBindRes = d.bindImageMemory(m_Image, m_Memory, 0);
        ASSERT(memoryBindRes == vk::Result::eSuccess, "Failed to bind image memory");

        vk::ImageViewCreateInfo viewInfo;
        viewInfo.image = m_Image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = m_Format;
        viewInfo.subresourceRange.aspectMask = m_Aspect;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseMipLevel = 0;

        vk::ResultValue<vk::ImageView> view = d.createImageView(viewInfo);
        ASSERT(view.result == vk::Result::eSuccess, "Failed to create image view");
        m_View = view.value;

        // TODO: Params
        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.minLod = -1000.0f;
        samplerInfo.maxLod = 1000.0f;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.anisotropyEnable = vk::False;

        vk::ResultValue<vk::Sampler> sampler = d.createSampler(samplerInfo);
        ASSERT(sampler.result == vk::Result::eSuccess, "Failed to create sampler");
        m_Sampler = sampler.value;
    }
} // namespace Mango::Driver::Vulkan
