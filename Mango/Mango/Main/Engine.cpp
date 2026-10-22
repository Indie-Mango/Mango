#include "Engine.hpp"
#include "Application.hpp"
#include "Mango/Base/TimeStep.hpp"

namespace Mango {
    Engine::Engine(std::vector<std::string>&& args, Arena& arena, AppCreateFunc appCreateFn) : m_Arena(arena), m_Args(std::move(args))
    {
        WindowContext::Create();
        m_Window = m_Arena.Create<Window>(this, WindowInfo{});
        m_App = appCreateFn(this, m_Args);
    }

    Engine::~Engine()
    {
        m_Arena.Destroy(m_Window);
        WindowContext::Destroy();
    }

    void Engine::Run()
    {
        TimeStep ts;

        while (m_App->IsRunning())
        {
            ts.Update();

            m_App->Update(ts.Time);

            // TODO: Render
            WindowContext::PollEvents();
            m_EventBus.Update();
        }
    }
} // namespace Mango
