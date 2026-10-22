#include "Mango/Mango.hpp"

#include <iostream>

namespace {
    class Sandbox : public Mango::Application
    {
    public:
        Sandbox(Mango::Engine* engine) : Application(engine)
        {
            windowClosedConnection =
                    engine->GetEventBus().Connect<Mango::WindowClosedEvent>([this](const Mango::WindowClosedEvent& e) {
                        m_Running = false;
                        return Mango::EventState::Continue;
                    });
        }

        void Update(Mango::F32 ts) override {}

        void Render(Mango::F32 ts, Mango::Renderer* renderer) const override {}

    private:
        Mango::EventConnection<Mango::WindowClosedEvent> windowClosedConnection;
    };

    Mango::Application* CreateApp(Mango::Engine* engine, [[maybe_unused]] const std::vector<std::string>& args)
    {
        return new Sandbox(engine);
    }
} // namespace

int main(int argc, char** argv) { return Mango::Main::Run(argc, argv, CreateApp); }
