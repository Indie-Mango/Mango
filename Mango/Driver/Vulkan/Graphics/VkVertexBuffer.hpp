#pragma once
#include "Mango/Graphics/VertexBuffer.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkRenderer;

    class VkVertexBuffer : public VertexBuffer
    {
    public:
        VkVertexBuffer(VkRenderer* renderer, const VertexBufferLayout& layout, U32 vertexCount, const void* data);
        ~VkVertexBuffer() override;

        VkVertexBuffer(const VkVertexBuffer&) = delete;
        VkVertexBuffer& operator=(const VkVertexBuffer&) = delete;

        VkVertexBuffer(VkVertexBuffer&&) = delete;
        VkVertexBuffer& operator=(VkVertexBuffer&&) = delete;

        void SetData(const void* data) override;

    private:
        void Allocate();

    private:
        VkRenderer* m_Renderer = nullptr;

        vk::Buffer m_Buffer{};
        vk::DeviceMemory m_Memory{};

        vk::Buffer m_StagingBuffer{};
        vk::DeviceMemory m_StagingBufferMemory{};
    };
} // namespace Mango::Driver::Vulkan
