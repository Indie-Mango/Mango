#pragma once
#include "Mango/Graphics/IndexBuffer.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkRenderer;

    class VkIndexBuffer : public IndexBuffer
    {
    public:
        VkIndexBuffer(VkRenderer* renderer, const std::vector<U32>& indices);
        ~VkIndexBuffer() override;

        VkIndexBuffer(const VkIndexBuffer&) = delete;
        VkIndexBuffer& operator=(const VkIndexBuffer&) = delete;

        VkIndexBuffer(VkIndexBuffer&&) = delete;
        VkIndexBuffer& operator=(VkIndexBuffer&&) = delete;

        void SetData(const std::vector<U32>& data) override;

        vk::Buffer GetBuffer() const { return m_Buffer; }

    private:
        VkRenderer* m_Renderer = nullptr;

        vk::Buffer m_Buffer{};
        vk::DeviceMemory m_Memory{};

        vk::Buffer m_StagingBuffer{};
        vk::DeviceMemory m_StagingBufferMemory{};
    };
} // namespace Mango::Driver::Vulkan
