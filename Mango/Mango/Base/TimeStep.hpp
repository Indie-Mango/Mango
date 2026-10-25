#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <chrono>

namespace Mango {
    struct TimeStep
    {
        F32 Time = 0;

        TimeStep() : m_LastTime(std::chrono::steady_clock::now()) {}
        void Update()
        {
            auto currentTime = std::chrono::steady_clock::now();
            Time = std::chrono::duration<F32>(currentTime - m_LastTime).count();
            m_LastTime = currentTime;
        }

    private:
        std::chrono::steady_clock::time_point m_LastTime;
    };
} // namespace Mango
