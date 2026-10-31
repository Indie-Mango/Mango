#include "Mango/Mango.hpp"

#include <imgui.h>

namespace {
    struct MVP
    {
        glm::mat4 Model{1.0f};
        glm::mat4 View{1.0f};
        glm::mat4 Proj{1.0f};
    };

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
                    Mango::ShaderDataType::Float3, // position
                    Mango::ShaderDataType::Float3 // color
            };

            Mango::F32 vertices[]{
                    0.0f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, // TOP
                    -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, // BOTTOM - LEFT
                    0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f // BOTTOM - RIGHT
            };

            std::vector<Mango::U32> indices = {0, 1, 2};

            m_VertexBuffer = Mango::VertexBuffer::Create(engine->GetRenderer(), vertexBufferLayout, 3, vertices);
            m_IndexBuffer = Mango::IndexBuffer::Create(engine->GetRenderer(), indices);

            Mango::PipelineInfo pipelineInfo;
            pipelineInfo.VertexBufferLayout = vertexBufferLayout;
            pipelineInfo.PipelineLayout = Mango::PipelineLayout{
                    .DescriptorSets = {Mango::PipelineDescriptorSet{.Descriptors = {Mango::PipelineDescriptorType::UniformBuffer},
                                                                    .Stage = Mango::PipelineStage::Vertex}}};

            Mango::File vertShader{"Sandbox/Assets/Shaders/Basic.vert", Mango::FileMode::Read};
            Mango::File fragShader{"Sandbox/Assets/Shaders/Basic.frag", Mango::FileMode::Read};
            m_pipeline = Mango::Pipeline::Create(engine->GetRenderer(), pipelineInfo, vertShader, fragShader);
            m_Material = Mango::Material::Create(engine->GetRenderer(), m_pipeline.Raw());
            m_UniformBuffer = Mango::UniformBuffer::Create(engine->GetRenderer(), 3 * sizeof(glm::mat4));

            Mango::Window* window = engine->GetWindow();
            m_MVP.Proj = glm::perspectiveZO(
                    45.0f, static_cast<Mango::F32>(window->GetWidth()) / static_cast<Mango::F32>(window->GetHeight()), 0.01f, 1000.0f);
            m_MVP.View = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.0f, -10.0f});
        }

        void Update(Mango::F32 ts) override
        {
            Mango::Window* window = m_Engine->GetWindow();
            if (window->IsKeyPressed(Mango::Key::W))
                m_Position.y += ts;
            if (window->IsKeyPressed(Mango::Key::S))
                m_Position.y -= ts;
            if (window->IsKeyPressed(Mango::Key::A))
                m_Position.x -= ts;
            if (window->IsKeyPressed(Mango::Key::D))
                m_Position.x += ts;

            m_MVP.Model = glm::translate(glm::mat4{1.0f}, m_Position);
        }

        void Render(Mango::F32 ts, Mango::Renderer* renderer) const override
        {
            m_UniformBuffer->Write(&m_MVP, sizeof(m_MVP));
            m_Material->SetBuffer(0, 0, m_UniformBuffer);
            m_Material->Update();
            m_Material->Bind();

            renderer->SetVertexBuffer(m_VertexBuffer);
            renderer->SetIndexBuffer(m_IndexBuffer);
            renderer->SetPipeline(m_pipeline);
            renderer->DrawIndexed(m_IndexBuffer->GetIndexCount());
        }

        void ImGuiRender(Mango::F32 ts) const override
        {
            ImGui::Begin("Engine data");
            ImGui::Value("Time step: ", ts);
            ImGui::End();
        }

    private:
        Mango::EventConnection<Mango::WindowClosedEvent> m_WindowClosedConnection;

        Mango::Shared<Mango::VertexBuffer> m_VertexBuffer;
        Mango::Shared<Mango::IndexBuffer> m_IndexBuffer;
        Mango::Shared<Mango::Pipeline> m_pipeline;
        Mango::Shared<Mango::Material> m_Material;
        Mango::Shared<Mango::UniformBuffer> m_UniformBuffer;
        MVP m_MVP;

        glm::vec3 m_Position{0.0f};
    };

    Mango::Application* CreateApp(Mango::Engine* engine, [[maybe_unused]] const std::vector<std::string>& args)
    {
        return new Sandbox(engine);
    }
} // namespace

int main(int argc, char** argv) { return Mango::Main::Run(argc, argv, CreateApp); }
