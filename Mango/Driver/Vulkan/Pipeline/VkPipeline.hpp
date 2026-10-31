#pragma once

#include "Mango/Core/Filesystem.hpp"
#include "Mango/Graphics/Pipeline.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkRenderer;

    class VkPipeline : public Pipeline
    {
    public:
        // TODO: Filesystem handle
        VkPipeline(VkRenderer* renderer, const PipelineInfo& info, const File& vertShader, const File& fragShader);
        ~VkPipeline() override;

        VkPipeline(const VkPipeline&) = delete;
        VkPipeline& operator=(const VkPipeline&) = delete;

        VkPipeline(VkPipeline&&) = delete;
        VkPipeline& operator=(VkPipeline&&) = delete;

        vk::Pipeline GetPipeline() const { return m_Pipeline; }
        vk::PipelineLayout GetLayout() const { return m_Layout; }

        const std::vector<vk::DescriptorSetLayout>& GetDescriptorSetLayouts() { return m_DescriptorSetLayouts; }

    private:
        void CreateDescriptorSets(const PipelineLayout& layout);
        void CreatePipelineLayout();
        void CreatePipeline(const PipelineInfo& info, const File& vertShader, const File& fragShader);

    private:
        VkRenderer* m_Renderer = nullptr;

        vk::Pipeline m_Pipeline;
        vk::PipelineLayout m_Layout;

        std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts;
    };
} // namespace Mango::Driver::Vulkan
