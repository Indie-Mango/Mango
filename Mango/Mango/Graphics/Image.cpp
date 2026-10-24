#include "Image.hpp"

#include "Driver/Vulkan/Graphics/VkImage.hpp"
#include "Driver/Vulkan/VkUtils.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    Shared<Image> Image::Create(Renderer* renderer, U32 width, U32 height, ImageType type, ImageUsage usage, const void* data)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return Shared<Driver::Vulkan::VkImage>::Create(reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer), width, height,
                                                               type, usage, data);
        }

        ASSERT(false, "Renderer uses invalid or unsupported API");
    }

    Image::Image(U32 width, U32 height) : m_Width(width), m_Height(height) {}
} // namespace Mango
