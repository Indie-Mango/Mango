#include "Engine.hpp"
#include "Application.hpp"
#include "Mango/Base/TimeStep.hpp"
#include "Mango/Core/Filesystem.hpp"
#include "Mango/Debug/Profile.hpp"
#include "Mango/ImGui/ImGuiContext.hpp"
#include "Mango/Renderer/Renderer.hpp"

#include <filesystem>

namespace Mango {
    Engine::Engine(std::vector<std::string>&& args, Arena& arena, AppCreateFunc appCreateFn) : m_Args(std::move(args)), m_Arena(arena)
    {
        FileSystem::CreateCacheDirectories("Mango-Cache");

        {
            MANGO_PROFILE_SCOPE_NAMED("Window context and window creation")
            WindowContext::Create();
            m_Window = m_Arena.Create<Window>(this, WindowInfo{});
        }

        m_Renderer = Renderer::Create(m_Window, m_EventBus);

        m_ImGuiContext = ImGuiContext::Create(m_Window, m_Renderer);

        {
            MANGO_PROFILE_SCOPE_NAMED("App creation")
            m_App = appCreateFn(this, m_Args);
        }
    }

    Engine::~Engine()
    {
        m_Renderer->WaitIdle();

        {
            MANGO_PROFILE_SCOPE_NAMED("App destruction")
            delete m_App;
        }

        {
            MANGO_PROFILE_SCOPE_NAMED("ImGui context destruction")
            delete m_ImGuiContext;
        }

        {
            MANGO_PROFILE_SCOPE_NAMED("Renderer destruction")
            delete m_Renderer;
        }

        {
            MANGO_PROFILE_SCOPE_NAMED("Window context and window destruction")
            m_Arena.Destroy(m_Window);
            WindowContext::Destroy();
        }
    }

    void Engine::Run()
    {
        TimeStep ts;

        while (m_App->IsRunning())
        {
            MANGO_PROFILE_SCOPE_NAMED("Frame")

            ts.Update();

            {
                MANGO_PROFILE_SCOPE_NAMED("Update")
                m_App->Update(ts.Time);
            }

            {
                MANGO_PROFILE_SCOPE_NAMED("Rendering")

                if (m_Window->CanRender() && m_Renderer->BeginFrame())
                {
                    m_App->Render(ts.Time, m_Renderer);

                    m_ImGuiContext->BeginFrame();
                    m_App->ImGuiRender(ts.Time);
                    m_ImGuiContext->EndFrame();

                    m_Renderer->EndFrame();
                }
            }

            {
                MANGO_PROFILE_SCOPE_NAMED("Event handling")
                WindowContext::PollEvents();
                m_EventBus.Update();
            }
        }
    }
} // namespace Mango
