#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Memory/Smart.hpp"
#include "Mango/Renderer/Renderer.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Mango {
    class IndexBuffer
    {
    public:
        static Shared<IndexBuffer> Create(Renderer* renderer, const std::vector<U32>& indices);
        virtual ~IndexBuffer() = default;

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;

        IndexBuffer(IndexBuffer&&) = delete;
        IndexBuffer& operator=(IndexBuffer&&) = delete;

        virtual void SetData(const std::vector<U32>& data) = 0;

    protected:
        IndexBuffer(const std::vector<U32>& indices);

    protected:
        USize m_IndexCount = 0;
        USize m_Size = 0;
    };
} // namespace Mango
