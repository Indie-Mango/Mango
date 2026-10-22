#include "Engine.hpp"
#include "Application.hpp"
#include "Mango/Base/TimeStep.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    Engine::Engine(std::vector<std::string>&& args, Arena& arena, AppCreateFunc appCreateFn) : m_Arena(arena), m_Args(std::move(args))
    {
        WindowContext::Create();
        m_Window = m_Arena.Create<Window>(this, WindowInfo{});

        m_Renderer = m_Arena.Create<Renderer>(m_Window);

        m_App = appCreateFn(this, m_Args);
    }

    Engine::~Engine()
    {
        delete m_App;

        m_Arena.Destroy(m_Renderer);

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

            if (!m_Window->IsMinimized() && m_Renderer->BeginFrame())
            {
                m_App->Render(ts.Time, m_Renderer);
                m_Renderer->EndFrame();
            }

            WindowContext::PollEvents();
            m_EventBus.Update();
        }
    }
} // namespace Mango
