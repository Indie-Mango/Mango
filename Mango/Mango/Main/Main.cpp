#include "Main.hpp"
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Debug/Profile.hpp"
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

        Arena arena(1'000'000);

        {
            MANGO_PROFILE_SCOPE_NAMED("Initialization")
            s_Engine = arena.Create<Engine>(std::move(args), arena, appCreateFn);
        }

        {
            MANGO_PROFILE_SCOPE_NAMED("Runtime")
            s_Engine->Run();
        }

        {
            MANGO_PROFILE_SCOPE_NAMED("Shutdown")
            arena.Destroy(s_Engine);
        }

        return EXIT_SUCCESS;
    }
} // namespace Mango
