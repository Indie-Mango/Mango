#pragma once

#if defined(MANGO_DEBUG)
#define ASSERT(expr, msg)                                                                                                             \
    do                                                                                                                                \
    {                                                                                                                                 \
        if (!(expr))                                                                                                                  \
        {                                                                                                                             \
            std::abort();                                                                                                             \
        }                                                                                                                             \
    } while (false)

#else
#define ASSERT(expr, msg)
#endif
