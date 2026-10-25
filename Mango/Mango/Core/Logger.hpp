#pragma once
#define MANGO_LOGGER_ENABLED 1
#include <format>
#include <iostream>

namespace Mango {
    namespace LoggerDetail {
        inline constexpr auto red = "\033[31m";
        inline constexpr auto yellow = "\033[33m";
        inline constexpr auto blue = "\033[34m";
        inline constexpr auto standard = "\033[31m";
    } // namespace LoggerDetail

    class Logger
    {
    public:
        Logger() = delete;
        ~Logger() = delete;

        Logger(const Logger& other) = delete;
        Logger& operator=(const Logger& other) = delete;

        Logger& operator=(Logger&& other) = delete;
        Logger(Logger&& other) = delete;

        template<typename... Args>
        static void Error(std::format_string<Args...> fmt, Args&&... args)
        {
#if MANGO_LOGGER_ENABLED
            std::cout << LoggerDetail::red << "[ERROR]: " << std::format(fmt, std::forward<Args>(args)...) << "\n"
                      << LoggerDetail::standard;
            std::cout.flush();
#endif
        }

        template<typename... Args>
        static void Warn(std::format_string<Args...> fmt, Args&&... args)
        {
#if MANGO_LOGGER_ENABLED
            std::cout << LoggerDetail::yellow << "[WARNING]: " << std::format(fmt, std::forward<Args>(args)...) << "\n"
                      << LoggerDetail::standard;
            std::cout.flush();
#endif
        }

        template<typename... Args>
        static void Info(std::format_string<Args...> fmt, Args&&... args)
        {
#if MANGO_LOGGER_ENABLED
            std::cout << LoggerDetail::blue << "[INFO]: " << std::format(fmt, std::forward<Args>(args)...) << "\n"
                      << LoggerDetail::standard;
#endif
        }
    };
} // namespace Mango
