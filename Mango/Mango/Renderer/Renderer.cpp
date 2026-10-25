#include "Renderer.hpp"

#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Mango/Core/Core.hpp"

namespace Mango {
    Renderer* Renderer::Create(const Window* window, EventBus& eventBus)
    {
        switch (m_API)
        {
            case API::Vulkan:
                return new Driver::Vulkan::VkRenderer(window, eventBus);
        }

        Logger::Error("Renderer uses invalid or unsupported API");
        MANGO_EXIT_PROGRAM();
    }

    void Renderer::OnWindowResize(const Window* window)
    {
        switch (m_API)
        {
            case API::Vulkan:
                return reinterpret_cast<Driver::Vulkan::VkRenderer*>(this)->OnWindowResizeVk(window);
        }
    }

    Renderer::Renderer(EventBus& eventBus)
    {
        m_WindowResizedConnection = eventBus.Connect<WindowResizedEvent>([this](const WindowResizedEvent& e) {
            OnWindowResize(e.Window);
            return EventState::Continue;
        });
    }
} // namespace Mango
