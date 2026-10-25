#pragma once
#include <iostream>

#if defined(MANGO_DEBUG)
#define ASSERT(expr, __VA_ARGS__)                                                                                                     \
    do                                                                                                                                \
    {                                                                                                                                 \
        if (!(expr))                                                                                                                  \
        {                                                                                                                             \
            Logger::Error(__VA_ARGS__);                                                                                               \
            std::abort();                                                                                                             \
        }                                                                                                                             \
    } while (false)

#else
#define ASSERT(expr, __VA_ARGS__)
#endif
