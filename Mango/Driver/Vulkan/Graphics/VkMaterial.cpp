#include "VkMaterial.hpp"

namespace Mango::Driver::Vulkan {
    VkMaterial::VkMaterial(VkRenderer* renderer, VkPipeline* pipeline) : m_Renderer(renderer), m_Pipeline(pipeline)
    {
        const std::vector<vk::DescriptorSetLayout>& layouts = m_Pipeline->GetDescriptorSetLayouts();
        m_Sets = m_Renderer->AllocateDescriptorSet(layouts);
    }
    
    void VkMaterial::Update()
    {
        for (auto& buffer: m_Buffers)
        {
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = reinterpret_cast<VkUniformBuffer*>(buffer.Buffer.Raw())->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = buffer.Buffer->Size();

            vk::WriteDescriptorSet write{};

            write.dstSet = m_Sets[0];
            write.dstBinding = 0;
            write.dstArrayElement = 0;
            write.descriptorType = vk::DescriptorType::eUniformBuffer;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            m_Renderer->GetDevice().updateDescriptorSets(write, nullptr);
        }
    }
    
    void VkMaterial::Bind()
    {
        m_Renderer->GetGraphicsCommandBufferRaw().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Pipeline->GetLayout(), 0,
                                                                     m_Sets, {});
    }
} // namespace Mango::Driver::Vulkan
