#include "Engine.hpp"
#include "Application.hpp"
#include "Mango/Base/TimeStep.hpp"
#include "Mango/Core/Filesystem.hpp"
#include "Mango/ImGui/ImGuiContext.hpp"
#include "Mango/Renderer/Renderer.hpp"

#include <filesystem>

namespace Mango {
    Engine::Engine(std::vector<std::string>&& args, Arena& arena, AppCreateFunc appCreateFn) : m_Args(std::move(args)), m_Arena(arena)
    {
        FileSystem::
        WindowContext::Create();
        m_Window = m_Arena.Create<Window>(this, WindowInfo{});

        m_Renderer = Renderer::Create(m_Window, m_EventBus);

        m_ImGuiContext = ImGuiContext::Create(m_Window, m_Renderer);

        m_App = appCreateFn(this, m_Args);
    }

    Engine::~Engine()
    {
        m_Renderer->WaitIdle();

        delete m_App;

        delete m_ImGuiContext;

        delete m_Renderer;

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

            if (m_Window->CanRender() && m_Renderer->BeginFrame())
            {
                m_App->Render(ts.Time, m_Renderer);

                m_ImGuiContext->BeginFrame();
                m_App->ImGuiRender(ts.Time);
                m_ImGuiContext->EndFrame();

                m_Renderer->EndFrame();
            }

            WindowContext::PollEvents();
            m_EventBus.Update();
        }
    }
} // namespace Mango
