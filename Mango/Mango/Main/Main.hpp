#pragma once
#include "Engine.hpp"

namespace Mango {
    class Main
    {
    public:
        Main() = delete;

        static int Run(int argc, char** argv, Engine::AppCreateFunc appCreateFn);
    };
} // namespace Mango
