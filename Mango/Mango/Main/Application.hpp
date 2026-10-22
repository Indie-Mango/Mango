#pragma once

namespace Mango {
    class Engine;

    class Application
    {
    public:
        Application(Engine* engine) : m_Engine(engine) {}

        virtual ~Application() = default;

        virtual void Update() = 0;
        virtual void Render() const = 0;

        bool IsRunning() const { return m_Running; }

    protected:
        Engine* m_Engine = nullptr;
        bool m_Running = true;
    };
} // namespace Mango
