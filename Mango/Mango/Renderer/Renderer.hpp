#pragma once
#include "Mango/Base/Window.hpp"

#include "Mango/Renderer/CommandBuffer.hpp"

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

namespace Mango {
    class Renderer
    {
    public:
        Renderer(const Window* window);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        bool BeginFrame();
        void EndFrame();

    private:
        void InitVulkan(GLFWwindow* window);
        void ShutdownVulkan();

    private:
        vk::Instance m_Instance = nullptr;
        vk::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
        vk::SurfaceKHR m_Surface = nullptr;
        vk::PhysicalDevice m_PhysicalDevice = nullptr;
        vk::Device m_Device = nullptr;
        vk::Queue m_GraphicsQueue = nullptr;
        vk::Queue m_PresentQueue = nullptr;
        vk::Queue m_TransferQueue = nullptr;

        vk::CommandPool m_CommandPool = nullptr;
        CommandBuffer m_Commandbuffer;

        U32 m_GraphicsQueueFamilyIdx = ~0;
        U32 m_PresentQueueFamilyIdx = ~0;
        U32 m_TransferQueueFamilyIdx = ~0;
    };
} // namespace Mango
