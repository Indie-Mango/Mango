#include "VkUtils.hpp"

#include "Mango/Core/Core.hpp"
#include "Mango/Core/Logger.hpp"

namespace Mango {
    U32 VkUtils::FindMemoryTypeIdx(U32 filter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice pd)
    {
        vk::PhysicalDeviceMemoryProperties memoryProperties = pd.getMemoryProperties();

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if (filter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        Logger::Error("Failed to find suitable memory type index");
        MANGO_EXIT_PROGRAM();
    }

    vk::SurfaceFormatKHR VkUtils::PickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
    {
        assert(!availableFormats.empty());
        const auto formatIt = std::ranges::find_if(availableFormats, [](const auto& format) {
            return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });

        return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
    }

    vk::Format VkUtils::ImageFormatToVk(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::R8Unorm:
                return vk::Format::eR8Unorm;
            case ImageFormat::RG8Unorm:
                return vk::Format::eR8G8Unorm;
            case ImageFormat::RGBA8Unorm:
                return vk::Format::eR8G8B8A8Unorm;
            case ImageFormat::RGBA8Srgb:
                return vk::Format::eR8G8B8A8Srgb;
            case ImageFormat::BGRA8Unorm:
                return vk::Format::eB8G8R8A8Unorm;
            case ImageFormat::BGRA8Srgb:
                return vk::Format::eB8G8R8A8Srgb;
            case ImageFormat::RGBA16Float:
                return vk::Format::eR16G16B16A16Sfloat;
            case ImageFormat::RGBA32Float:
                return vk::Format::eR32G32B32A32Sfloat;
        }

        Logger::Error("Invalid image format");
        MANGO_EXIT_PROGRAM();
    }

    ImageFormat VkUtils::VkFormatToImageFormat(vk::Format format)
    {
        switch (format)
        {
            case vk::Format::eR8Unorm:
                return ImageFormat::R8Unorm;
            case vk::Format::eR8G8Unorm:
                return ImageFormat::RG8Unorm;
            case vk::Format::eR8G8B8A8Unorm:
                return ImageFormat::RGBA8Unorm;
            case vk::Format::eR8G8B8A8Srgb:
                return ImageFormat::RGBA8Srgb;
            case vk::Format::eB8G8R8A8Unorm:
                return ImageFormat::BGRA8Unorm;
            case vk::Format::eB8G8R8A8Srgb:
                return ImageFormat::BGRA8Srgb;
            case vk::Format::eR16G16B16A16Sfloat:
                return ImageFormat::RGBA16Float;
            case vk::Format::eR32G32B32A32Sfloat:
                return ImageFormat::RGBA32Float;
            default:
                break;
        }

        Logger::Error("Vulkan format {} has no matching image format", vk::to_string(format));
        MANGO_EXIT_PROGRAM();
    }

    void VkUtils::TransitionImageLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout from, vk::ImageLayout to,
                                        vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 srcAccess,
                                        vk::AccessFlags2 dstAccess, vk::ImageAspectFlags aspect, vk::Image image)
    {
        vk::ImageMemoryBarrier2 barrier;
        barrier.image = image;
        barrier.oldLayout = from;
        barrier.newLayout = to;
        barrier.srcStageMask = srcStage;
        barrier.dstStageMask = dstStage;
        barrier.srcAccessMask = srcAccess;
        barrier.dstAccessMask = dstAccess;
        barrier.subresourceRange.aspectMask = aspect;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        vk::DependencyInfo depInfo;
        depInfo.dependencyFlags = {};
        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = &barrier;

        commandBuffer.pipelineBarrier2(depInfo);
    }
} // namespace Mango
