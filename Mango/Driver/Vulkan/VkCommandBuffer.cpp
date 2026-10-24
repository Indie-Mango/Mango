#include "VkCommandBuffer.hpp"

#include "Mango/Core/Core.hpp"

namespace Mango::Driver::Vulkan {
    VkCommandBuffer::VkCommandBuffer(vk::Device d, vk::CommandPool cmdPool) : m_Device(d), m_CommandPool(cmdPool)
    {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1u;
        vk::Result res = m_Device.allocateCommandBuffers(&allocInfo, &m_Buffer);
        ASSERT(res == vk::Result::eSuccess, "Failed to allocate command buffers");
    }

    VkCommandBuffer::~VkCommandBuffer()
    {
        if (m_Device)
            m_Device.freeCommandBuffers(m_CommandPool, 1u, &m_Buffer);
    }

    VkCommandBuffer::VkCommandBuffer(VkCommandBuffer&& other) noexcept :
        m_Device(std::exchange(other.m_Device, nullptr)), m_CommandPool(std::exchange(other.m_CommandPool, nullptr)),
        m_Buffer(std::exchange(other.m_Buffer, nullptr))
    {
    }

    VkCommandBuffer& VkCommandBuffer::operator=(VkCommandBuffer&& other) noexcept
    {
        if (this == &other)
            return *this;

        m_Device = std::exchange(other.m_Device, nullptr);
        m_CommandPool = std::exchange(other.m_CommandPool, nullptr);
        m_Buffer = std::exchange(other.m_Buffer, nullptr);

        return *this;
    }

    void VkCommandBuffer::Begin() const
    {
        vk::CommandBufferBeginInfo beginInfo;
        vk::Result beginRes = m_Buffer.begin(beginInfo);
        ASSERT(beginRes == vk::Result::eSuccess, "Failed to begin recording command buffer");
    }

    void VkCommandBuffer::End() const
    {
        vk::Result endRes = m_Buffer.end();
        ASSERT(endRes == vk::Result::eSuccess, "Failed to end command buffer");
    }

    void VkCommandBuffer::BeginRendering(vk::ImageView imageView, vk::Extent2D extent) const
    {
        vk::RenderingAttachmentInfo renderingAttachment;
        renderingAttachment.imageView = imageView;
        renderingAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        renderingAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        renderingAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        renderingAttachment.clearValue = vk::ClearValue{vk::ClearColorValue{0.8f, 0.0f, 0.8f, 1.0f}};

        vk::RenderingInfo renderingInfo;
        renderingInfo.renderArea = vk::Rect2D{vk::Offset2D{0, 0}, extent};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &renderingAttachment;

        m_Buffer.beginRendering(renderingInfo);
    }

    void VkCommandBuffer::EndRendering() const { m_Buffer.endRendering(); }
} // namespace Mango::Driver::Vulkan
