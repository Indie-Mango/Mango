#pragma once
#include "Mango/Base/Window.hpp"

#include "Mango/Base/Event.hpp"
#include "Mango/Memory/Smart.hpp"

namespace Mango {
    class Image;

    class Renderer
    {
    public:
        enum class API : U8
        {
            Vulkan
        };

    public:
        static Renderer* Create(const Window* window, EventBus& eventBus);
        virtual ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void WaitIdle() = 0;

        void OnWindowResize(const Window* window);

        static API GetAPI() { return m_API; }

    protected:
        Renderer(EventBus& eventBus);

    protected:
        Shared<Image> m_TargetImage;

    private:
        EventConnection<WindowResizedEvent> m_WindowResizedConnection;
        inline static auto m_API = API::Vulkan;
    };
} // namespace Mango
