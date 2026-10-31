#include "VkUniformBuffer.hpp"

#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Driver/Vulkan/VkUtils.hpp"
#include "Mango/Core/Core.hpp"

namespace Mango::Driver::Vulkan {
    VkUniformBuffer::VkUniformBuffer(VkRenderer* renderer, USize size) : m_Renderer(renderer), m_Size(size)
    {
        vk::PhysicalDevice pd = m_Renderer->GetPhysicalDevice();
        vk::Device d = m_Renderer->GetDevice();

        vk::BufferCreateInfo bufferInfo;
        bufferInfo.size = m_Size;
        bufferInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ResultValue<vk::Buffer> buffer = d.createBuffer(bufferInfo);
        ASSERT(buffer.result == vk::Result::eSuccess, "Failed to create index buffer");
        m_Buffer = buffer.value;

        vk::MemoryRequirements memoryRequirements = d.getBufferMemoryRequirements(m_Buffer);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex =
                VkUtils::FindMemoryTypeIdx(memoryRequirements.memoryTypeBits,
                                           vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pd);

        vk::ResultValue<vk::DeviceMemory> memory = d.allocateMemory(allocInfo);
        ASSERT(memory.result == vk::Result::eSuccess, "Failed to allocate memory for index buffer");
        m_Memory = memory.value;

        vk::Result bindRes = d.bindBufferMemory(m_Buffer, m_Memory, 0);
        ASSERT(bindRes == vk::Result::eSuccess, "Failed to bind buffer memory");
    }

    VkUniformBuffer::~VkUniformBuffer()
    {
        vk::Device d = m_Renderer->GetDevice();

        d.destroyBuffer(m_Buffer);
        d.free(m_Memory);
    }

    void VkUniformBuffer::Write(const void* data, USize size)
    {
        ASSERT(size <= m_Size, "Given size must be smaller or equal to buffer size");

        vk::Device d = m_Renderer->GetDevice();

        vk::ResultValue<void*> mappedRes = d.mapMemory(m_Memory, 0, m_Size);
        ASSERT(mappedRes.result == vk::Result::eSuccess, "Failed to map buffer memory");
        void* mapped = mappedRes.value;
        memcpy(mapped, data, size);
        d.unmapMemory(m_Memory);
    }
} // namespace Mango::Driver::Vulkan
