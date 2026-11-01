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

                Logger::Error("Invalid image type");
                MANGO_EXIT_PROGRAM();
            }

            vk::ImageUsageFlags MangoImageUsageToVulkanImageUsage(ImageUsage usage)
            {
                switch (usage)
                {
                    case ImageUsage::ColorAttachment:
                        return vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment;
                    case ImageUsage::Texture:
                        return vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
                }

                Logger::Error("Invalid image usage");
                MANGO_EXIT_PROGRAM();
            }

        } // namespace Utils
    } // namespace

    VkImage::VkImage(VkRenderer* renderer, U32 width, U32 height, ImageFormat format, ImageType type, ImageUsage usage,
                     const void* data) :
        Image(width, height, format), m_Renderer(renderer), m_MangoUsage(usage),
        m_Usage(Utils::MangoImageUsageToVulkanImageUsage(usage)), m_VkFormat(VkUtils::ImageFormatToVk(format)),
        m_Aspect(Utils::MangoImageTypeToVulkanImageAspect(type))
    {
        ASSERT(IsFormatSupported(), "Image format {} is not supported for this usage on this device", vk::to_string(m_VkFormat));

        CreateSampler();
        Allocate();

        if (data)
            VkImage::SetData(data);
    }

    VkImage::~VkImage()
    {
        Free();
        m_Renderer->GetDevice().destroySampler(m_Sampler);
    }

    void VkImage::Resize(U32 width, U32 height)
    {
        if (width == 0 || height == 0)
            return;

        if (m_Width == width && m_Height == height)
            return;

        m_Width = width;
        m_Height = height;

        // NOTE: Contents are lost, a texture has to be re-uploaded with SetData
        Free();
        Allocate();
    }

    void VkImage::TransitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout from, vk::ImageLayout to,
                                   vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 srcAccess,
                                   vk::AccessFlags2 dstAccess) const
    {
        VkUtils::TransitionImageLayout(commandBuffer, from, to, srcStage, dstStage, srcAccess, dstAccess, m_Aspect, m_Image);
    }

    void VkImage::SetData(const void* data)
    {
        ASSERT(m_Usage & vk::ImageUsageFlagBits::eTransferDst, "Image was not created with an usage that allows uploading data");

        vk::PhysicalDevice pd = m_Renderer->GetPhysicalDevice();
        vk::Device d = m_Renderer->GetDevice();

        const USize size = GetDataSize();

        // Staging buffer only lives for the duration of the upload
        vk::BufferCreateInfo stagingBufferInfo;
        stagingBufferInfo.size = size;
        stagingBufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
        stagingBufferInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ResultValue<vk::Buffer> stagingBufferRes = d.createBuffer(stagingBufferInfo);
        ASSERT(stagingBufferRes.result == vk::Result::eSuccess, "Failed to create staging buffer");
        vk::Buffer stagingBuffer = stagingBufferRes.value;

        vk::MemoryRequirements memoryRequirements = d.getBufferMemoryRequirements(stagingBuffer);

        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex =
                VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits,
                                           vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pd);

        vk::ResultValue<vk::DeviceMemory> stagingMemoryRes = d.allocateMemory(allocInfo);
        ASSERT(stagingMemoryRes.result == vk::Result::eSuccess, "Failed to allocate memory for staging buffer");
        vk::DeviceMemory stagingMemory = stagingMemoryRes.value;

        vk::Result memoryBindRes = d.bindBufferMemory(stagingBuffer, stagingMemory, 0);
        ASSERT(memoryBindRes == vk::Result::eSuccess, "Failed to bind staging buffer memory to buffer");

        vk::ResultValue<void*> mapRes = d.mapMemory(stagingMemory, 0, size);
        ASSERT(mapRes.result == vk::Result::eSuccess, "Failed to map memory of staging buffer");
        memcpy(mapRes.value, data, size);
        d.unmapMemory(stagingMemory);

        vk::CommandBuffer transferCommandBuffer = m_Renderer->GetTransferCommandBufferRaw();

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        vk::Result beginRes = transferCommandBuffer.begin(beginInfo);
        ASSERT(beginRes == vk::Result::eSuccess, "Failed to begin transfer command buffer recording");

        // Previous contents are discarded
        TransitionLayout(transferCommandBuffer, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
                         vk::PipelineStageFlagBits2::eNone, vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eNone,
                         vk::AccessFlagBits2::eTransferWrite);

        vk::BufferImageCopy region;
        region.imageSubresource.aspectMask = m_Aspect;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageExtent = vk::Extent3D{m_Width, m_Height, 1};
        transferCommandBuffer.copyBufferToImage(stagingBuffer, m_Image, vk::ImageLayout::eTransferDstOptimal, region);

        // The transfer queue does not support shader stages, so dstStage stays eNone here.
        // Visibility for the graphics queue is guaranteed by waiting on the transfer fence below.
        TransitionLayout(transferCommandBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
                         vk::PipelineStageFlagBits2::eCopy, vk::PipelineStageFlagBits2::eNone, vk::AccessFlagBits2::eTransferWrite,
                         vk::AccessFlagBits2::eNone);

        vk::Result endRes = transferCommandBuffer.end();
        ASSERT(endRes == vk::Result::eSuccess, "Failed to end transfer command buffer recording");

        m_Renderer->SubmitTransfer();
        m_Renderer->WaitForTransfer();

        d.destroyBuffer(stagingBuffer);
        d.free(stagingMemory);
    }

    void VkImage::Allocate()
    {
        vk::PhysicalDevice pd = m_Renderer->GetPhysicalDevice();
        vk::Device d = m_Renderer->GetDevice();

        vk::ImageCreateInfo imageInfo;
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.format = m_VkFormat;
        imageInfo.extent = vk::Extent3D{m_Width, m_Height, 1};
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = vk::SampleCountFlagBits::e1;
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.usage = m_Usage;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;

        // Textures are written on the (dedicated) transfer queue and read on the graphics queue.
        // Concurrent sharing avoids having to do queue family ownership transfers.
        std::array queueFamilies = {m_Renderer->GetGraphicsQueueFamilyIdx(), m_Renderer->GetTransferQueueFamilyIdx()};
        if (m_MangoUsage == ImageUsage::Texture && queueFamilies[0] != queueFamilies[1])
        {
            imageInfo.sharingMode = vk::SharingMode::eConcurrent;
            imageInfo.queueFamilyIndexCount = static_cast<U32>(queueFamilies.size());
            imageInfo.pQueueFamilyIndices = queueFamilies.data();
        }
        else
        {
            imageInfo.sharingMode = vk::SharingMode::eExclusive;
        }

        vk::ResultValue<vk::Image> imageRes = d.createImage(imageInfo);
        ASSERT(imageRes.result == vk::Result::eSuccess, "Failed to create vulkan image");
        m_Image = imageRes.value;

        vk::MemoryRequirements memoryRequirements = d.getImageMemoryRequirements(m_Image);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex =
                VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, pd);
        vk::ResultValue<vk::DeviceMemory> memory = d.allocateMemory(allocInfo);
        ASSERT(memory.result == vk::Result::eSuccess, "Failed to allocate memory for image");
        m_Memory = memory.value;

        vk::Result memoryBindRes = d.bindImageMemory(m_Image, m_Memory, 0);
        ASSERT(memoryBindRes == vk::Result::eSuccess, "Failed to bind image memory");

        vk::ImageViewCreateInfo viewInfo;
        viewInfo.image = m_Image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = m_VkFormat;
        viewInfo.subresourceRange.aspectMask = m_Aspect;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseMipLevel = 0;

        vk::ResultValue<vk::ImageView> view = d.createImageView(viewInfo);
        ASSERT(view.result == vk::Result::eSuccess, "Failed to create image view");
        m_View = view.value;
    }

    void VkImage::Free()
    {
        vk::Device d = m_Renderer->GetDevice();

        d.destroyImageView(m_View);
        d.destroyImage(m_Image);
        d.free(m_Memory);

        m_View = nullptr;
        m_Image = nullptr;
        m_Memory = nullptr;
    }

    // Independent of the image size, so it survives a Resize
    void VkImage::CreateSampler()
    {
        // TODO: Params
        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = vk::LodClampNone;
        samplerInfo.anisotropyEnable = vk::False;
        samplerInfo.maxAnisotropy = 1.0f;

        vk::ResultValue<vk::Sampler> sampler = m_Renderer->GetDevice().createSampler(samplerInfo);
        ASSERT(sampler.result == vk::Result::eSuccess, "Failed to create sampler");
        m_Sampler = sampler.value;
    }

    USize VkImage::GetDataSize() const
    {
        return static_cast<USize>(m_Width) * m_Height * ImageFormatBytesPerPixel(m_Format);
    }

    bool VkImage::IsFormatSupported() const
    {
        vk::FormatProperties properties = m_Renderer->GetPhysicalDevice().getFormatProperties(m_VkFormat);
        vk::FormatFeatureFlags features = properties.optimalTilingFeatures;

        if ((m_Usage & vk::ImageUsageFlagBits::eSampled) && !(features & vk::FormatFeatureFlagBits::eSampledImage))
            return false;
        if ((m_Usage & vk::ImageUsageFlagBits::eColorAttachment) && !(features & vk::FormatFeatureFlagBits::eColorAttachment))
            return false;
        if ((m_Usage & vk::ImageUsageFlagBits::eTransferDst) && !(features & vk::FormatFeatureFlagBits::eTransferDst))
            return false;
        if ((m_Usage & vk::ImageUsageFlagBits::eTransferSrc) && !(features & vk::FormatFeatureFlagBits::eTransferSrc))
            return false;

        return true;
    }
} // namespace Mango::Driver::Vulkan
