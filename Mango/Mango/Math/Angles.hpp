#pragma once

namespace Mango::Math {
    constexpr F32 PI = 3.14159265359;
    constexpr F32 TWO_PI = 2 * PI;

    F32 Radians(F32 degrees);
    F32 Degrees(F32 radians);
    
    F32 Sin(F32 radians);
    F32 Cos(F32 radians);
} // namespace Mango::Math