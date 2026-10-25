#include "Mango/Mango.hpp"

#include <iostream>

namespace {
    class Sandbox : public Mango::Application
    {
    public:
        Sandbox(Mango::Engine* engine) : Application(engine)
        {
            m_WindowClosedConnection =
                    engine->GetEventBus().Connect<Mango::WindowClosedEvent>([this](const Mango::WindowClosedEvent& e) {
                        m_Running = false;
                        return Mango::EventState::Continue;
                    });

            Mango::VertexBufferLayout vertexBufferLayout = {
                    Mango::ShaderDataType::Float3 // position
            };

            float vertices[]{
                    0.0f,  0.5f,  1.0f, // TOP
                    -0.5f, -0.5f, 1.0f, // BOTTOM - LEFT
                    0.5f,  -0.5f, 1.0f, // BOTTOM - RIGHT
            };

            std::vector<Mango::U32> indices = {0, 1, 2};

            m_VertexBuffer = Mango::VertexBuffer::Create(engine->GetRenderer(), vertexBufferLayout, 3, vertices);
            m_IndexBuffer = Mango::IndexBuffer::Create(engine->GetRenderer(), indices);

            Mango::PipelineInfo pipelineInfo;
            pipelineInfo.VertexBufferLayout = vertexBufferLayout;

            m_pipeline = Mango::Pipeline::Create(engine->GetRenderer(), pipelineInfo, "Sandbox/Assets/Shaders/Basic.vert",
                                                 "Sandbox/Assets/Shaders/Basic.frag");
        }

        void Update(Mango::F32 ts) override {}

        void Render(Mango::F32 ts, Mango::Renderer* renderer) const override {}

    private:
        Mango::EventConnection<Mango::WindowClosedEvent> m_WindowClosedConnection;

        Mango::Shared<Mango::VertexBuffer> m_VertexBuffer;
        Mango::Shared<Mango::IndexBuffer> m_IndexBuffer;
        Mango::Shared<Mango::Pipeline> m_pipeline;
    };

    Mango::Application* CreateApp(Mango::Engine* engine, [[maybe_unused]] const std::vector<std::string>& args)
    {
        return new Sandbox(engine);
    }
} // namespace

int main(int argc, char** argv) { return Mango::Main::Run(argc, argv, CreateApp); }
