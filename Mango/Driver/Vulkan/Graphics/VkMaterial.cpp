#include "VkMaterial.hpp"
#include "Driver/Vulkan/Graphics/VkUniformBuffer.hpp"
#include "Driver/Vulkan/Pipeline/VkPipeline.hpp"

#include "Driver/Vulkan/Graphics/VkImage.hpp"
#include "Mango/Debug/Profile.hpp"

namespace Mango::Driver::Vulkan {
    VkMaterial::VkMaterial(VkRenderer* renderer, PipelineHandle pipeline) : m_Renderer(renderer), m_Pipeline(pipeline)
    {
        MANGO_PROFILE_SCOPE()
        
        auto* rawPipeline = reinterpret_cast<VkPipeline*>(m_Renderer->GetPipelineManager().ToRaw(m_Pipeline));
        const std::vector<vk::DescriptorSetLayout>& layouts = rawPipeline->GetDescriptorSetLayouts();
        m_Sets = m_Renderer->AllocateDescriptorSet(layouts);
    }

    void VkMaterial::Update()
    {
        MANGO_PROFILE_SCOPE()
        
        for (auto& buffer : m_Buffers)
        {
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = reinterpret_cast<VkUniformBuffer*>(buffer.Buffer.Raw())->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = buffer.Buffer->Size();

            vk::WriteDescriptorSet write{};

            write.dstSet = m_Sets[buffer.Set];
            write.dstBinding = buffer.Binding;
            write.dstArrayElement = 0;
            write.descriptorType = vk::DescriptorType::eUniformBuffer;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            m_Renderer->GetDevice().updateDescriptorSets(write, nullptr);
        }

        for (auto& texture : m_Textures)
        {
            auto* image = reinterpret_cast<VkImage*>(texture.Texture.GetImage().Raw());

            vk::DescriptorImageInfo imageInfo;
            imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfo.imageView = image->GetView();
            imageInfo.sampler = image->GetSampler();

            vk::WriteDescriptorSet write{};
            write.dstSet = m_Sets[texture.Set];
            write.dstBinding = texture.Binding;
            write.dstArrayElement = 0;
            write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            write.descriptorCount = 1;
            write.pImageInfo = &imageInfo;

            m_Renderer->GetDevice().updateDescriptorSets(write, nullptr);
        }
    }

    void VkMaterial::Bind()
    {
        auto* rawPipeline = reinterpret_cast<VkPipeline*>(m_Renderer->GetPipelineManager().ToRaw(m_Pipeline));
        m_Renderer->GetGraphicsCommandBufferRaw().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, rawPipeline->GetLayout(), 0,
                                                                     m_Sets, {});
    }
} // namespace Mango::Driver::Vulkan
