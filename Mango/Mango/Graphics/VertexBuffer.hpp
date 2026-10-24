#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Memory/Smart.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango {
    class Renderer;

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
        static Shared<VertexBuffer> Create(Renderer* renderer, const VertexBufferLayout& layout, U32 vertexCount, const void* data);

        virtual ~VertexBuffer() = default;

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        VertexBuffer(VertexBuffer&&) = delete;
        VertexBuffer& operator=(VertexBuffer&&) = delete;

        virtual void SetData(const void* data) = 0;

        const VertexBufferLayout& GetVertexBufferLayout() const { return m_Layout; }

        USize GetSize() const { return m_Size; }
        U32 GetVertexCount() const { return m_VertexCount; }

    protected:
        VertexBuffer(const VertexBufferLayout& layout, U32 vertexCount) :
            m_Layout(layout), m_VertexCount(vertexCount), m_Size(layout.Stride * vertexCount)
        {
        }

    protected:
        VertexBufferLayout m_Layout;

        USize m_Size = 0;
        U32 m_VertexCount = 0;
    };
} // namespace Mango
