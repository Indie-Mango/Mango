#include "ImGuiContext.hpp"

#include "Driver/Vulkan/ImGui/VkImGuiContext.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Core/Logger.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    ImGuiContext* ImGuiContext::Create(Window* window, Renderer* renderer)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return new Driver::Vulkan::VkImGuiContext(window, reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer));
        }

        Logger::Error("Renderer uses invalid or unsupported API");
        MANGO_EXIT_PROGRAM();
    }
} // namespace Mango
