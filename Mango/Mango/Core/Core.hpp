#pragma once
#include <iostream>

#if defined(MANGO_DEBUG)
#define ASSERT(expr, msg)                                                                                                             \
    do                                                                                                                                \
    {                                                                                                                                 \
        if (!(expr))                                                                                                                  \
        {                                                                                                                             \
            std::cout << msg << std::endl;                                                                                            \
            std::abort();                                                                                                             \
        }                                                                                                                             \
    } while (false)

#else
#define ASSERT(expr, msg)
#endif
