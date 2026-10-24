#include "IndexBuffer.hpp"

#include "Mango/Core/Core.hpp"
#include "Mango/Graphics/VkUtils.hpp"

namespace Mango {
    IndexBuffer::IndexBuffer(vk::PhysicalDevice pd, vk::Device d, vk::CommandBuffer transferCommandBuffer,
                             const std::vector<U32>& indices) :
        m_PhysicalDevice(pd), m_Device(d), m_TransferCommandBuffer(transferCommandBuffer), m_IndexCount(indices.size()),
        m_Size(indices.size() * sizeof(U32))
    {
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.size = m_Size;
        bufferInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ResultValue<vk::Buffer> buffer = m_Device.createBuffer(bufferInfo);
        ASSERT(buffer.result == vk::Result::eSuccess, "Failed to create index buffer");
        m_Buffer = buffer.value;

        vk::MemoryRequirements memoryRequirements = m_Device.getBufferMemoryRequirements(m_Buffer);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits,
                                                               vk::MemoryPropertyFlagBits::eDeviceLocal, m_PhysicalDevice);

        vk::ResultValue<vk::DeviceMemory> memory = m_Device.allocateMemory(allocInfo);
        ASSERT(memory.result == vk::Result::eSuccess, "Failed to allocate memory for index buffer");
        m_Memory = memory.value;

        if (!indices.empty())
            SetData(indices);
    }
    
    IndexBuffer::~IndexBuffer()
    {
        m_Device.destroyBuffer(m_StagingBuffer);
        m_Device.free(m_StagingBufferMemory);

        m_Device.destroyBuffer(m_Buffer);
        m_Device.free(m_Memory);
    }
    
    void IndexBuffer::SetData(const std::vector<U32>& data)
    {
        ASSERT(data.size() * sizeof(U32) == m_Size, "Index buffer data must be of same size");

        if (!m_StagingBuffer)
        {
            vk::BufferCreateInfo bufferInfo;
            bufferInfo.size = m_Size;
            bufferInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;

            vk::ResultValue<vk::Buffer> buffer = m_Device.createBuffer(bufferInfo);
            ASSERT(buffer.result == vk::Result::eSuccess, "Failed to create staging buffer");
            m_StagingBuffer = buffer.value;

            vk::MemoryRequirements memoryRequirements = m_Device.getBufferMemoryRequirements(m_StagingBuffer);
            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memoryRequirements.size;
            allocInfo.memoryTypeIndex = VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits,
                                                                   vk::MemoryPropertyFlagBits::eDeviceLocal, m_PhysicalDevice);

            vk::ResultValue<vk::DeviceMemory> memory = m_Device.allocateMemory(allocInfo);
            ASSERT(memory.result == vk::Result::eSuccess, "Failed to allocate memory for staging buffer");
            m_StagingBufferMemory = memory.value;
        }

        vk::ResultValue<void*> mappedRes = m_Device.mapMemory(m_StagingBufferMemory, 0, m_Size);
        ASSERT(mappedRes.result == vk::Result::eSuccess, "Failed to map staging buffer memory");
        void* mapped = mappedRes.value;
        memcpy(mapped, data.data(), m_Size);
        m_Device.unmapMemory(m_StagingBufferMemory);

        vk::BufferCopy region;
        region.size = m_Size;
        region.srcOffset = 0;
        region.dstOffset = 0;
        m_TransferCommandBuffer.copyBuffer(m_StagingBuffer, m_Buffer, region);
    }
} // namespace Mango
