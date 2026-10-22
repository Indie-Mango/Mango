#include "Mango/Mango.hpp"

#include <iostream>

struct NameEvent
{
    std::string Name;
};

class Sandbox : public Mango::Application
{
public:
    Sandbox(Mango::Engine* engine) : Application(engine)
    {
        con = engine->GetEventBus().Connect<NameEvent>([](const NameEvent& e) {
            std::cout << e.Name << "\n";
            return Mango::EventState::Continue;
        });
    }

    void Update() override
    {
        //
        m_Engine->GetEventBus().Emit<NameEvent>(NameEvent{"hello"});
    }

    void Render() const override {}

private:
    Mango::EventConnection<NameEvent> con;
};

Mango::Application* CreateApp(Mango::Engine* engine, [[maybe_unused]] const std::vector<std::string>& args)
{
    return new Sandbox(engine);
}

int main(int argc, char** argv) { return Mango::Main::Run(argc, argv, CreateApp); }
