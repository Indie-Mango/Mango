#pragma once
#include "Mango/Base/Event.hpp"
#include "Mango/Memory/Arena.hpp"

#include <string>
#include <vector>

namespace Mango {
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

        Application* m_App;

        EventBus m_EventBus;
    };
} // namespace Mango
