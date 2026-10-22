#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <string>

struct GLFWwindow;

namespace Mango {
    class Engine;
    class EventBus;
    struct WindowInfo
    {
        std::string Title = "Mango";
        U32 Width = 1280, Height = 720;
    };

    class Window
    {
    public:
        Window(Engine* engine, const WindowInfo& info);
        ~Window();

        Window(const Window&) = delete;
        Window operator=(const Window&) = delete;

        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        const std::string& GetTitle() { return m_Data.Title; }
        void SetTitle(const std::string& title);

        U32 GetWidth() const { return m_Data.Width; }
        U32 GetHeight() const { return m_Data.Height; }

        void SetWidth(U32 width);
        void SetHeight(U32 height);
        void SetSize(U32 width, U32 height);

        void SetPos(S32 xPos, S32 yPos);

        void Close() const;
        bool IsOpen() const { return m_Data.Open; }
        bool IsFocused() const { return m_Data.Focused; }

        bool IsMinimized() const { return m_Data.Minimized; }

        GLFWwindow* GetNativeWindow() const;

    private:
        void SetEventCallbackFunctions() const;

    private:
        GLFWwindow* m_Window = nullptr;

        struct Data
        {
            Window* Self = nullptr;
            EventBus* EventBus = nullptr;

            std::string Title;

            U32 Width, Height;
            S32 XPos = 0, YPos = 0;

            bool Open = true;
            bool Focused = true;
            bool Minimized = false;

            Data(const WindowInfo& info) : Title(info.Title), Width(info.Width), Height(info.Height) {}
        };

        Data m_Data;
    };

    struct WindowContext
    {
        static bool Create();
        static void Destroy();

        static void PollEvents();
    };

    struct WindowClosedEvent
    {
        Window* Window;
    };

    struct WindowMovedEvent
    {
        Window* Window;
    };

    struct WindowResizedEvent
    {
        Window* Window;
    };

    struct WindowFocusedEvent
    {
        Window* Window;
    };

    struct WindowMinimizedEvent
    {
        Window* Window;
    };
} // namespace Mango
