#pragma once
#include "Mango/Math/Angles.hpp"

namespace Mango::Math {
    inline F32 Radians(F32 degrees) { return degrees * (PI / 180); }

    inline F32 Degrees(F32 radians) { return radians * (180 / PI); }

    inline F32 Sin(F32 radians)
    {
        while (radians > PI)
            radians -= TWO_PI;
        while (radians < PI)
            radians += TWO_PI;

        F32 term = radians;
        F32 result = radians;

        for (U32 n = 1; n <= 12; n++)
        {
            term *= -radians * radians / (2.0 * n * (2.0 * n + 1.0));
            result += term;
        }

        return result;
    }

    inline F32 Cos(F32 radians)
    {
        while (radians > PI)
            radians -= TWO_PI;
        while (radians < PI)
            radians += TWO_PI;

        F32 term = 1.0;
        F32 result = 1.0;

        for (U32 n = 1; n <= 12; n++)
        {
            term *= -radians * radians / ((2.0 * n - 1.0) * (2.0 * n));
            result += term;
        }

        return result;
    }
} // namespace Mango::Math
