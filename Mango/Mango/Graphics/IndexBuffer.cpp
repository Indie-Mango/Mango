#include "IndexBuffer.hpp"

#include "Driver/Vulkan/Graphics/VkIndexBuffer.hpp"
#include "Driver/Vulkan/VkUtils.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    Shared<IndexBuffer> IndexBuffer::Create(Renderer* renderer, const std::vector<U32>& indices)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return Shared<Driver::Vulkan::VkIndexBuffer>::Create(reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer), indices);
        }

        ASSERT(false, "Renderer uses invalid or unsupported API");
    }

    void IndexBuffer::SetData(const std::vector<U32>& data) {}

    IndexBuffer::IndexBuffer(const std::vector<U32>& indices) : m_IndexCount(indices.size()), m_Size(indices.size() * sizeof(U32)) {}
} // namespace Mango
