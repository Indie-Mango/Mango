#include "Mango/Mango.hpp"

#include <iostream>

namespace {
    class Sandbox : public Mango::Application
    {
    public:
        Sandbox(Mango::Engine* engine) : Application(engine) {}

        void Update() override { m_Running = false; }

        void Render() const override {}
    };

    Mango::Application* CreateApp(Mango::Engine* engine, [[maybe_unused]] const std::vector<std::string>& args)
    {
        return new Sandbox(engine);
    }
} // namespace

int main(int argc, char** argv) { return Mango::Main::Run(argc, argv, CreateApp); }
