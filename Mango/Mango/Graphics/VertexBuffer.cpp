#include "VertexBuffer.hpp"

#include "Driver/Vulkan/Graphics/VkVertexBuffer.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Debug/Profile.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    Shared<VertexBuffer> VertexBuffer::Create(Renderer* renderer, const VertexBufferLayout& layout, U32 vertexCount, const void* data)
    {
        MANGO_PROFILE_SCOPE()
        
        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return Shared<Driver::Vulkan::VkVertexBuffer>::Create(reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer), layout,
                                                                      vertexCount, data);
        }

        Logger::Error("Renderer uses invalid or unsupported API");
        MANGO_EXIT_PROGRAM();
    }
} // namespace Mango
