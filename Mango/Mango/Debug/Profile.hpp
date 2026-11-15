#pragma once

#include "tracy/Tracy.hpp"

#if MANGO_TRACY_ENABLED
#define MANGO_PROFILE_SCOPE() ZoneScoped
#define MANGO_PROFILE_SCOPE_NAMED(name) ZoneScopedN(name)
#else
#define MANGO_PROFILE_SCOPE()
#define MANGO_PROFILE_SCOPE_NAMED(name)
#endif
