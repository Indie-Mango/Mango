#pragma once
#include "Mango/Core/Logger.hpp"

#define MANGO_EXIT_PROGRAM() std::abort()

#if defined(MANGO_DEBUG)
#define ASSERT(expr, ...)                                                                                                             \
    do                                                                                                                                \
    {                                                                                                                                 \
        if (!(expr))                                                                                                                  \
        {                                                                                                                             \
            Logger::Error(__VA_ARGS__);                                                                                               \
            MANGO_EXIT_PROGRAM();                                                                                                     \
        }                                                                                                                             \
    } while (false)
#else
#define ASSERT(expr, ...)
#endif