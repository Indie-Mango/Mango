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

            Mango::Renderer* renderer = engine->GetRenderer();

            Mango::ResourceManager& resourceManager = engine->GetResourceManager();
            Mango::ResourceHandle pictureResource =
                    resourceManager.CreateResource<Mango::Picture>("Sandbox/Assets/Textures/Smiley.png");
            if (!resourceManager.LoadResource(pictureResource))
            {
                MANGO_EXIT_PROGRAM();
            }

            m_Texture = Mango::Texture{engine, pictureResource};
            resourceManager.Release(pictureResource);

            Mango::PipelineInfo pipelineInfo;

            // clang-format off
            Mango::PipelineLayout layout{
            .DescriptorSets = {
            // Set 0: Renderer / Scene
            {
                .Descriptors = {
                    {
                        .Binding = 0,
                        .Type = Mango::PipelineDescriptorType::UniformBuffer
                    }, // Camera

                    {
                        .Binding = 1,
                        .Type = Mango::PipelineDescriptorType::UniformBuffer
                    }, // Object / Model

                    {
                        .Binding = 2,
                        .Type = Mango::PipelineDescriptorType::UniformBuffer
                    }, // Lights
                },
                .Set = 0,
                .Stage = Mango::PipelineStage::All
            },

            // Set 1: Material
            {
                .Descriptors = {
                    {
                        .Binding = 0,
                        .Type = Mango::PipelineDescriptorType::UniformBuffer
                    }, // Material PBR properties

                    {
                        .Binding = 1,
                        .Type = Mango::PipelineDescriptorType::Texture
                    }, // Base color

                    {
                        .Binding = 2,
                        .Type = Mango::PipelineDescriptorType::Texture
                    }, // Metallic / Roughness

                    {
                        .Binding = 3,
                        .Type = Mango::PipelineDescriptorType::Texture
                    }, // Normal map

                    {
                        .Binding = 4,
                        .Type = Mango::PipelineDescriptorType::Texture
                    }, // Occlusion

                    {
                        .Binding = 5,
                        .Type = Mango::PipelineDescriptorType::Texture
                    }, // Emissive
                },
                .Set = 1,
                .Stage = Mango::PipelineStage::Fragment
            }
                }
            };
            // clang-format on

            pipelineInfo.PipelineLayout = layout;

            Mango::PipelineManager& pipelineManager = renderer->GetPipelineManager();
            Mango::MaterialManager& materialManager = renderer->GetMaterialManager();

            Mango::File vertShader{"Sandbox/Assets/Shaders/Basic.vert", Mango::FileMode::Read};
            Mango::File fragShader{"Sandbox/Assets/Shaders/Basic.frag", Mango::FileMode::Read};
            m_pbrPipeline = pipelineManager.CreatePipeline(engine->GetRenderer(), pipelineInfo, vertShader, fragShader);

            m_Material = materialManager.CreateMaterial(engine->GetRenderer(), m_pbrPipeline);
            m_UniformBuffer = Mango::UniformBuffer::Create(engine->GetRenderer(), 3 * sizeof(glm::mat4));
            materialManager.SetUniformBuffer(m_Material, 0, 0, m_UniformBuffer);
            materialManager.SetTexture(m_Material, 1, 0, m_Texture);

            Mango::Window* window = engine->GetWindow();
            m_MVP.Proj = glm::perspectiveZO(
                    45.0f, static_cast<Mango::F32>(window->GetWidth()) / static_cast<Mango::F32>(window->GetHeight()), 0.01f, 1000.0f);
            m_MVP.View = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.0f, -10.0f});
            m_MVP.View[1][1] *= -1;

            m_MVP.Model = glm::translate(glm::mat4{1.0f}, {0.0f, 0.0f, -5.0f});
        }

        ~Sandbox() override
        {
            m_Engine->GetRenderer()->GetMaterialManager().Release(m_Material);

            m_Engine->GetRenderer()->GetPipelineManager().Release(m_pbrPipeline);
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

            Mango::MaterialManager& materialManager = renderer->GetMaterialManager();
            materialManager.UpdateMaterials();

            renderer->SetPipeline(m_pbrPipeline);

            materialManager.Bind(m_Material);
        }

        void ImGuiRender(Mango::F32 ts) const override
        {
            ImGui::Begin("Engine data");
            ImGui::Value("Time step: ", ts);
            ImGui::End();
        }

    private:
        Mango::EventConnection<Mango::WindowClosedEvent> m_WindowClosedConnection;

        Mango::PipelineHandle m_pbrPipeline;
        Mango::MaterialHandle m_Material;
        Mango::Shared<Mango::UniformBuffer> m_UniformBuffer;
        Mango::Texture m_Texture;
        MVP m_MVP;

        glm::vec3 m_Position{0.0f};
    };

    Mango::Application* CreateApp(Mango::Engine* engine, [[maybe_unused]] const std::vector<std::string>& args)
    {
        return new Sandbox(engine);
    }
} // namespace

int main(int argc, char** argv) { return Mango::Main::Run(argc, argv, CreateApp); }
