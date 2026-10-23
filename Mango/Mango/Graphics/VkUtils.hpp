#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Core/Core.hpp"

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

namespace Mango {
    struct VkUtils
    {
        static U32 FindMemoryTypeIdx(U32 filter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice pd);
        static vk::SurfaceFormatKHR PickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
        
        static void TransitionImageLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout from, vk::ImageLayout to,
                                          vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage,
                                          vk::AccessFlags2 srcAccess, vk::AccessFlags2 dstAccess, vk::ImageAspectFlags aspect,
                                          vk::Image image);
    };
} // namespace Mango
