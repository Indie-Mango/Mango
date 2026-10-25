#include "VkVertexBuffer.hpp"

#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Driver/Vulkan/VkUtils.hpp"
#include "Mango/Core/Core.hpp"

namespace Mango::Driver::Vulkan {
    VkVertexBuffer::VkVertexBuffer(VkRenderer* renderer, const VertexBufferLayout& layout, U32 vertexCount, const void* data) :
        VertexBuffer(layout, vertexCount), m_Renderer(renderer)
    {
        Allocate();
        if (data)
            VkVertexBuffer::SetData(data);
    }

    VkVertexBuffer::~VkVertexBuffer()
    {
        vk::Device d = m_Renderer->GetDevice();

        d.destroyBuffer(m_StagingBuffer);
        d.free(m_StagingBufferMemory);

        d.destroyBuffer(m_Buffer);
        d.free(m_Memory);
    }

    void VkVertexBuffer::SetData(const void* data)
    {
        vk::PhysicalDevice pd = m_Renderer->GetPhysicalDevice();
        vk::Device d = m_Renderer->GetDevice();
        vk::CommandBuffer transferCommandBuffer = m_Renderer->GetTransferCommandBufferRaw();

        if (!m_StagingBuffer)
        {
            vk::BufferCreateInfo stagingBufferInfo;
            stagingBufferInfo.size = m_Size;
            stagingBufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            stagingBufferInfo.sharingMode = vk::SharingMode::eExclusive;

            vk::ResultValue<vk::Buffer> stagingBuffer = d.createBuffer(stagingBufferInfo);
            ASSERT(stagingBuffer.result == vk::Result::eSuccess, "Failed to create staging buffer");
            m_StagingBuffer = stagingBuffer.value;

            vk::MemoryRequirements memoryRequirements = d.getBufferMemoryRequirements(m_StagingBuffer);
            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memoryRequirements.size;
            allocInfo.memoryTypeIndex = VkUtils::FindMemoryTypeIdx(
                    memoryRequirements.memoryTypeBits,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pd);

            vk::ResultValue<vk::DeviceMemory> memoryRes = d.allocateMemory(allocInfo);
            ASSERT(memoryRes.result == vk::Result::eSuccess, "Failed to allocate memory for staging buffer");
            m_StagingBufferMemory = memoryRes.value;

            vk::Result memoryBindRes = d.bindBufferMemory(m_StagingBuffer, m_StagingBufferMemory, 0);
            ASSERT(memoryBindRes == vk::Result::eSuccess, "Failed to bind memory to staging buffer");
        }

        vk::ResultValue<void*> mappedRes = d.mapMemory(m_StagingBufferMemory, 0, m_Size);
        ASSERT(mappedRes.result == vk::Result::eSuccess, "Failed to map memory to staging buffer");
        void* mapped = mappedRes.value;
        memcpy(mapped, data, m_Size);
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

    void VkVertexBuffer::Allocate()
    {
        vk::PhysicalDevice pd = m_Renderer->GetPhysicalDevice();
        vk::Device d = m_Renderer->GetDevice();

        vk::BufferCreateInfo bufferInfo;
        bufferInfo.size = m_Size;
        bufferInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ResultValue<vk::Buffer> buffer = d.createBuffer(bufferInfo);
        ASSERT(buffer.result == vk::Result::eSuccess, "Failed to create vertex buffer");
        m_Buffer = buffer.value;

        vk::MemoryRequirements memoryRequirements = d.getBufferMemoryRequirements(m_Buffer);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex =
                VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, pd);

        vk::ResultValue<vk::DeviceMemory> memoryRes = d.allocateMemory(allocInfo);
        ASSERT(memoryRes.result == vk::Result::eSuccess, "Failed to allocate memory for vertex buffer");
        m_Memory = memoryRes.value;

        vk::Result memoryBindRes = d.bindBufferMemory(m_Buffer, m_Memory, 0);
        ASSERT(memoryBindRes == vk::Result::eSuccess, "Failed to bind memory to vertex buffer");
    }
} // namespace Mango::Driver::Vulkan
