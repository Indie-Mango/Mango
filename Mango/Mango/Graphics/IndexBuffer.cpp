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

        Logger::Error("Renderer uses invalid or unsupported API");
        MANGO_EXIT_PROGRAM();
    }

    void IndexBuffer::SetData(const std::vector<U32>& data) {}

    IndexBuffer::IndexBuffer(const std::vector<U32>& indices) :
        m_Size(indices.size() * sizeof(U32)), m_IndexCount(static_cast<U32>(indices.size()))
    {
    }
} // namespace Mango
