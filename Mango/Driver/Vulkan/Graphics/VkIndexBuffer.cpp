#include "VkIndexBuffer.hpp"

#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Driver/Vulkan/VkUtils.hpp"
#include "Mango/Core/Core.hpp"

namespace Mango::Driver::Vulkan {
    VkIndexBuffer::VkIndexBuffer(VkRenderer* renderer, const std::vector<U32>& indices) : IndexBuffer(indices), m_Renderer(renderer)
    {
        vk::PhysicalDevice pd = renderer->GetPhysicalDevice();
        vk::Device d = renderer->GetDevice();

        vk::BufferCreateInfo bufferInfo;
        bufferInfo.size = m_Size;
        bufferInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ResultValue<vk::Buffer> buffer = d.createBuffer(bufferInfo);
        ASSERT(buffer.result == vk::Result::eSuccess, "Failed to create index buffer");
        m_Buffer = buffer.value;

        vk::MemoryRequirements memoryRequirements = d.getBufferMemoryRequirements(m_Buffer);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex =
                VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, pd);

        vk::ResultValue<vk::DeviceMemory> memory = d.allocateMemory(allocInfo);
        ASSERT(memory.result == vk::Result::eSuccess, "Failed to allocate memory for index buffer");
        m_Memory = memory.value;

        vk::Result bindRes = d.bindBufferMemory(m_Buffer, m_Memory, 0);
        ASSERT(bindRes == vk::Result::eSuccess, "Failed to bind buffer memory");

        if (!indices.empty())
            VkIndexBuffer::SetData(indices);
    }

    VkIndexBuffer::~VkIndexBuffer()
    {
        vk::Device d = m_Renderer->GetDevice();

        d.destroyBuffer(m_StagingBuffer);
        d.free(m_StagingBufferMemory);

        d.destroyBuffer(m_Buffer);
        d.free(m_Memory);
    }

    void VkIndexBuffer::SetData(const std::vector<U32>& data)
    {
        vk::PhysicalDevice pd = m_Renderer->GetPhysicalDevice();
        vk::Device d = m_Renderer->GetDevice();
        vk::CommandBuffer transferCommandBuffer = m_Renderer->GetTransferCommandBufferRaw();

        ASSERT(data.size() * sizeof(U32) == m_Size, "Index buffer data must be of same size");

        if (!m_StagingBuffer)
        {
            vk::BufferCreateInfo bufferInfo;
            bufferInfo.size = m_Size;
            bufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;

            vk::ResultValue<vk::Buffer> buffer = d.createBuffer(bufferInfo);
            ASSERT(buffer.result == vk::Result::eSuccess, "Failed to create staging buffer");
            m_StagingBuffer = buffer.value;

            vk::MemoryRequirements memoryRequirements = d.getBufferMemoryRequirements(m_StagingBuffer);
            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memoryRequirements.size;
            allocInfo.memoryTypeIndex = VkUtils::FindMemoryTypeIdx(
                    memoryRequirements.memoryTypeBits,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pd);

            vk::ResultValue<vk::DeviceMemory> memory = d.allocateMemory(allocInfo);
            ASSERT(memory.result == vk::Result::eSuccess, "Failed to allocate memory for staging buffer");
            m_StagingBufferMemory = memory.value;

            vk::Result bindRes = d.bindBufferMemory(m_StagingBuffer, m_StagingBufferMemory, 0);
            ASSERT(bindRes == vk::Result::eSuccess, "Failed to bind staging buffer memory");
        }

        vk::ResultValue<void*> mappedRes = d.mapMemory(m_StagingBufferMemory, 0, m_Size);
        ASSERT(mappedRes.result == vk::Result::eSuccess, "Failed to map staging buffer memory");
        void* mapped = mappedRes.value;
        memcpy(mapped, data.data(), m_Size);
        d.unmapMemory(m_StagingBufferMemory);

        vk::CommandBufferBeginInfo beginInfo;
        vk::Result transferBeginRes = transferCommandBuffer.begin(beginInfo);
        ASSERT(transferBeginRes == vk::Result::eSuccess, "Failed to begin recording transfer command buffer");

        vk::BufferCopy region;
        region.size = m_Size;
        region.srcOffset = 0;
        region.dstOffset = 0;

        transferCommandBuffer.copyBuffer(m_StagingBuffer, m_Buffer, region);

        vk::Result transferEndRes = transferCommandBuffer.end();
        ASSERT(transferEndRes == vk::Result::eSuccess, "Failed to end recording transfer command buffer");

        m_Renderer->SubmitTransfer();
    }
} // namespace Mango::Driver::Vulkan
