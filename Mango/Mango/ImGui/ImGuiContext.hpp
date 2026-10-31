#pragma once
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    class ImGuiContext
    {
    public:
        static ImGuiContext* Create(Window* window, Renderer* renderer);
        virtual ~ImGuiContext() = default;

        ImGuiContext(const ImGuiContext&) = delete;
        ImGuiContext& operator=(const ImGuiContext&) = delete;

        ImGuiContext(ImGuiContext&&) = delete;
        ImGuiContext& operator=(ImGuiContext&&) = delete;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

    protected:
        ImGuiContext() = default;
    };
} // namespace Mango
