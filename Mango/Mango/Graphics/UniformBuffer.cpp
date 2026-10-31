#include "UniformBuffer.hpp"

#include "Driver/Vulkan/Graphics/VkUniformBuffer.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    Shared<UniformBuffer> UniformBuffer::Create(Renderer* renderer, USize size)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return Shared<Driver::Vulkan::VkUniformBuffer>::Create(reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer), size);
        }

        Logger::Error("Renderer uses invalid or unsupported API");
        MANGO_EXIT_PROGRAM();
    }
} // namespace Mango
