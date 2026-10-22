#include "Engine.hpp"
#include "Application.hpp"

namespace Mango {
    Engine::Engine(std::vector<std::string>&& args, Arena& arena, AppCreateFunc appCreateFn) : m_Arena(arena), m_Args(std::move(args))
    {
        m_App = appCreateFn(this, m_Args);
    }

    Engine::~Engine() = default;

    void Engine::Run()
    {
        while (m_App->IsRunning())
        {
            m_App->Update();
            
            // TODO: Render
            
            m_EventBus.Update();
        }
    }
} // namespace Mango
