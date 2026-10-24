#include "VertexBuffer.hpp"

#include "Driver/Vulkan/Graphics/VkVertexBuffer.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    Shared<VertexBuffer> VertexBuffer::Create(Renderer* renderer, const VertexBufferLayout& layout, U32 vertexCount, const void* data)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return Shared<Driver::Vulkan::VkVertexBuffer>::Create(reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer), layout,
                                                                      vertexCount, data);
        }

        ASSERT(false, "Renderer uses invalid or unsupported API");
    }
} // namespace Mango
