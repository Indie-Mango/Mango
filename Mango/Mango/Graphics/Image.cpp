#include "Image.hpp"

#include "Mango/Core/Core.hpp"
#include "Mango/Core/Logger.hpp"

#include "Driver/Vulkan/Graphics/VkImage.hpp"
#include "Driver/Vulkan/VkUtils.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    Shared<Image> Image::Create(Renderer* renderer, U32 width, U32 height, ImageFormat format, ImageType type, ImageUsage usage,
                                const void* data)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return Shared<Driver::Vulkan::VkImage>::Create(reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer), width, height,
                                                               format, type, usage, data);
        }

        Logger::Error("Renderer uses invalid or unsupported API");
        MANGO_EXIT_PROGRAM();
    }

    Image::Image(U32 width, U32 height, ImageFormat format) : m_Width(width), m_Height(height), m_Format(format) {}
} // namespace Mango
