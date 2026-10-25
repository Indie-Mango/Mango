#pragma once
#include "Mango/Memory/Smart.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    class ImGuiContext
    {
    public:
        static ImGuiContext* Create(Window* window, Renderer* renderer);
        virtual ~ImGuiContext() = default;

        ImGuiContext(const ImGuiContext& other) = delete;
        ImGuiContext& operator=(const ImGuiContext& other) = delete;

        ImGuiContext(ImGuiContext&& other) = delete;
        ImGuiContext& operator=(ImGuiContext&& other) = delete;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

    protected:
        ImGuiContext() = default;
    };
} // namespace Mango
