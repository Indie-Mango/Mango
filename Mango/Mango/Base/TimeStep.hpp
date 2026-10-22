#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <chrono>

namespace Mango {
    struct TimeStep
    {
        F32 Time = 0;

        TimeStep() : m_LastTime(std::chrono::system_clock::now()) {}
        void Update()
        {
            auto currentTime = std::chrono::system_clock::now();
            Time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_LastTime).count();
            m_LastTime = currentTime;
        }

    private:
        std::chrono::system_clock::time_point m_LastTime;
    };
} // namespace Mango
