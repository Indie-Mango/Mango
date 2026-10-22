#pragma once
#include "Mango/Base/Event.hpp"
#include "Mango/Base/Window.hpp"
#include "Mango/Memory/Arena.hpp"

#include <string>
#include <vector>

namespace Mango {
    class Renderer;
    class Application;

    class Engine
    {
    public:
        using AppCreateFunc = Application*(Engine* engine, const std::vector<std::string>& args);

    public:
        Engine(std::vector<std::string>&& args, Arena& arena, AppCreateFunc appCreateFn);
        ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

        void Run();

        Arena& GetArena() const { return m_Arena; }
        EventBus& GetEventBus() { return m_EventBus; }

    private:
        std::vector<std::string> m_Args;

        Arena& m_Arena;
        EventBus m_EventBus;

        Application* m_App = nullptr;
        Window* m_Window = nullptr;
        Renderer* m_Renderer = nullptr;
    };
} // namespace Mango
