#include "Main.hpp"
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Memory/Arena.hpp"

namespace Mango {
    namespace {
        inline Engine* s_Engine = nullptr;
    }

    int Main::Run(int argc, char** argv, Engine::AppCreateFunc appCreateFn)
    {
        std::vector<std::string> args(argc);
        for (S32 i = 0; i < argc; i++)
        {
            args[i] = argv[i];
        }

        Arena arena(2048);
        s_Engine = arena.Create<Engine>(std::move(args), arena, appCreateFn);
        s_Engine->Run();
        arena.Destroy(s_Engine);

        return EXIT_SUCCESS;
    }
} // namespace Mango
