#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango {
    enum class VertexBufferElement : U8
    {
        Float,
        Float2,
        Float3,
        Float4,
    };

    inline USize VertexBufferElementSize(VertexBufferElement e)
    {
        switch (e)
        {
            case VertexBufferElement::Float:
                return 1 * static_cast<USize>(4u);
            case VertexBufferElement::Float2:
                return 2 * static_cast<USize>(4);
            case VertexBufferElement::Float3:
                return 3 * static_cast<USize>(4);
            case VertexBufferElement::Float4:
                return 4 * static_cast<USize>(4u);
        }

        return 0;
    }

    struct VertexBufferLayout
    {
        VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements) : Elements(elements)
        {
            Count = Elements.size();
            for (auto element: Elements)
                Stride += VertexBufferElementSize(element);
        }

        USize Count = 0;
        USize Stride = 0;
        std::vector<VertexBufferElement> Elements;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(vk::PhysicalDevice pd, vk::Device d, vk::CommandBuffer transferCommandBuffer, const VertexBufferLayout& layout,
                     USize vertexCount, const void* data);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        VertexBuffer(VertexBuffer&&) = delete;
        VertexBuffer& operator=(VertexBuffer&&) = delete;

        void SetData(const void* data);

        const VertexBufferLayout& GetVertexBufferLayout() const { return m_Layout; }
        USize GetVertexCount() const { return m_VertexCount; }
        USize GetSize() const { return m_Size; }

    private:
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_Device;

        vk::CommandBuffer m_TransferCommandBuffer;

        vk::Buffer m_Buffer;
        vk::DeviceMemory m_Memory;

        VertexBufferLayout m_Layout;

        USize m_VertexCount = 0;
        USize m_Size = 0;

        vk::Buffer m_StagingBuffer;
        vk::DeviceMemory m_StagingBufferMemory;
    };
} // namespace Mango
