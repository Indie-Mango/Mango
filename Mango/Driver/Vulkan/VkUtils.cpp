#include "VkUtils.hpp"

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
