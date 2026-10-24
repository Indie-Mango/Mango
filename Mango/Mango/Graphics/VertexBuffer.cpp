#include "VertexBuffer.hpp"

#include "Mango/Core/Core.hpp"
#include "VkUtils.hpp"

namespace Mango {
    VertexBuffer::VertexBuffer(vk::PhysicalDevice pd, vk::Device d, vk::CommandBuffer transferCommandBuffer,
                               const VertexBufferLayout& layout, USize vertexCount, const void* data) :
        m_PhysicalDevice(pd), m_Device(d), m_TransferCommandBuffer(transferCommandBuffer), m_VertexCount(vertexCount),
        m_Size(layout.Stride * vertexCount), m_Layout(layout)
    {
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.size = m_Size;
        bufferInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ResultValue<vk::Buffer> buffer = m_Device.createBuffer(bufferInfo);
        ASSERT(buffer.result == vk::Result::eSuccess, "Failed to create vertex buffer");
        m_Buffer = buffer.value;

        vk::MemoryRequirements memoryRequirements = m_Device.getBufferMemoryRequirements(m_Buffer);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits,
                                                               vk::MemoryPropertyFlagBits::eDeviceLocal, m_PhysicalDevice);

        vk::ResultValue<vk::DeviceMemory> memoryRes = m_Device.allocateMemory(allocInfo);
        ASSERT(memoryRes.result == vk::Result::eSuccess, "Failed to allocate memory for vertex buffer");
        m_Memory = memoryRes.value;

        vk::Result memoryBindRes = m_Device.bindBufferMemory(m_Buffer, m_Memory, 0);
        ASSERT(memoryBindRes == vk::Result::eSuccess, "Failed to bind memory to vertex buffer");

        if (data)
            SetData(data);
    }
    
    VertexBuffer::~VertexBuffer()
    {
        m_Device.destroyBuffer(m_StagingBuffer);
        m_Device.free(m_StagingBufferMemory);

        m_Device.destroyBuffer(m_Buffer);
        m_Device.free(m_Memory);
    }

    void VertexBuffer::SetData(const void* data)
    {
        if (!m_StagingBuffer)
        {
            vk::BufferCreateInfo stagingBufferInfo;
            stagingBufferInfo.size = m_Size;
            stagingBufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            stagingBufferInfo.sharingMode = vk::SharingMode::eExclusive;

            vk::ResultValue<vk::Buffer> stagingBuffer = m_Device.createBuffer(stagingBufferInfo);
            ASSERT(stagingBuffer.result == vk::Result::eSuccess, "Failed to create staging buffer");
            m_StagingBuffer = stagingBuffer.value;

            vk::MemoryRequirements memoryRequirements = m_Device.getBufferMemoryRequirements(m_StagingBuffer);
            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memoryRequirements.size;
            allocInfo.memoryTypeIndex = VkUtils::FindMemoryTypeIdx(
                    memoryRequirements.memoryTypeBits,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, m_PhysicalDevice);

            vk::ResultValue<vk::DeviceMemory> memoryRes = m_Device.allocateMemory(allocInfo);
            ASSERT(memoryRes.result == vk::Result::eSuccess, "Failed to allocate memory for staging buffer");
            m_StagingBufferMemory = memoryRes.value;

            vk::Result memoryBindRes = m_Device.bindBufferMemory(m_StagingBuffer, m_StagingBufferMemory, 0);
            ASSERT(memoryBindRes == vk::Result::eSuccess, "Failed to bind memory to staging buffer");
        }

        vk::ResultValue<void*> mappedRes = m_Device.mapMemory(m_StagingBufferMemory, 0, m_Size);
        ASSERT(mappedRes.result == vk::Result::eSuccess, "Failed to map memory to staging buffer");
        void* mapped = mappedRes.value;
        memcpy(mapped, data, m_Size);
        m_Device.unmapMemory(m_Memory);

        vk::BufferCopy region;
        region.size = m_Size;
        region.srcOffset = 0;
        region.dstOffset = 0;
        m_TransferCommandBuffer.copyBuffer(m_StagingBuffer, m_Buffer, region);
    }
} // namespace Mango
