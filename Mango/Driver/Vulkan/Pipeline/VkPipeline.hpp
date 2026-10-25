#pragma once

#include "Mango/Graphics/Pipeline.hpp"

#include <string>
#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkRenderer;

    class VkPipeline : public Pipeline
    {
    public:
        // TODO: Filesystem handle
        VkPipeline(VkRenderer* renderer, const PipelineInfo& info, const std::string& vertShader, const std::string& fragShader);
        ~VkPipeline() override;

        VkPipeline(const VkPipeline&) = delete;
        VkPipeline& operator=(const VkPipeline&) = delete;

        VkPipeline(VkPipeline&&) = delete;
        VkPipeline& operator=(VkPipeline&&) = delete;

        vk::Pipeline GetPipeline() const { return m_Pipeline; }

    private:
        VkRenderer* m_Renderer = nullptr;

        vk::Pipeline m_Pipeline;
        vk::PipelineLayout m_Layout;
    };
} // namespace Mango::Driver::Vulkan
