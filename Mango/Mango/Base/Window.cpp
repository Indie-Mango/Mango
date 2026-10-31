#include "Window.hpp"

#include "Mango/Core/Core.hpp"
#include "Mango/Main/Engine.hpp"

#include <GLFW/glfw3.h>

namespace Mango {
    Window::Window(Engine* engine, const WindowInfo& info) : m_Data(info)
    {
        m_Data.Self = this;
        m_Data.EventBus = &engine->GetEventBus();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_Window = glfwCreateWindow(static_cast<int>(m_Data.Width), static_cast<int>(m_Data.Height), m_Data.Title.c_str(), nullptr,
                                    nullptr);

        ASSERT(m_Window, "Failed to create window");

        glfwGetWindowPos(m_Window, &m_Data.XPos, &m_Data.YPos);
        glfwSetWindowUserPointer(m_Window, &m_Data);

        SetEventCallbackFunctions();
    }

    Window::~Window()
    {
        if (m_Data.Open)
        {
            Close();
        }
    }

    bool Window::IsKeyPressed(Key::Code key) const { return glfwGetKey(m_Window, key); }

    bool Window::IsMouseButtonPressed(Mouse::Button button) const { return glfwGetMouseButton(m_Window, button); }

    glm::vec2 Window::GetMousePos() const
    {
        glm::dvec2 pos;
        glfwGetCursorPos(m_Window, &pos.x, &pos.y);
        return pos;
    }

    void Window::SetTitle(const std::string& title)
    {
        m_Data.Title = title;
        glfwSetWindowTitle(m_Window, m_Data.Title.c_str());
    }

    void Window::SetWidth(U32 width)
    {
        m_Data.Width = width;
        glfwSetWindowSize(m_Window, static_cast<int>(m_Data.Width), static_cast<int>(m_Data.Height));
    }

    void Window::SetHeight(U32 height)
    {
        m_Data.Height = height;
        glfwSetWindowSize(m_Window, static_cast<int>(m_Data.Width), static_cast<int>(m_Data.Height));
    }

    void Window::SetSize(U32 width, U32 height)
    {
        m_Data.Width = width;
        m_Data.Height = height;
        glfwSetWindowSize(m_Window, static_cast<int>(m_Data.Width), static_cast<int>(m_Data.Height));
    }

    void Window::SetPos(S32 xPos, S32 yPos)
    {
        m_Data.XPos = xPos;
        m_Data.YPos = yPos;
        glfwSetWindowPos(m_Window, m_Data.XPos, m_Data.YPos);
    }

    void Window::Close() const { glfwDestroyWindow(m_Window); }

    GLFWwindow* Window::GetNativeWindow() const { return m_Window; }

    void Window::SetEventCallbackFunctions() const
    {
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            auto* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
            data->Open = false;
            data->EventBus->Emit(WindowClosedEvent{});
        });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xPos, int yPos) {
            auto* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
            data->XPos = xPos;
            data->YPos = yPos;
            data->EventBus->Emit(WindowMovedEvent{data->Self});
        });

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            auto* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
            data->Width = width;
            data->Height = height;
            data->EventBus->Emit(WindowResizedEvent{data->Self});
        });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
            auto* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
            data->Focused = focused;
            data->EventBus->Emit(WindowFocusedEvent{data->Self});
        });

        glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified) {
            auto* data = static_cast<Data*>(glfwGetWindowUserPointer(window));
            data->Minimized = iconified;
            data->EventBus->Emit(WindowMinimizedEvent{data->Self});
        });
    }

    bool WindowContext::Create() { return glfwInit(); }

    void WindowContext::Destroy() { glfwTerminate(); }

    void WindowContext::PollEvents() { glfwPollEvents(); }
} // namespace Mango
