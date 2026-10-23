#pragma once
#include "Mango/Base/Window.hpp"

#include "Mango/Renderer/CommandBuffer.hpp"

#include "Mango/Base/Event.hpp"
#include "Mango/Memory/Smart.hpp"
#include "Mango/Renderer/Swapchain.hpp"

#define VULKAN_HPP_NO_EXCEPTIONS
#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

namespace Mango {
    class Image;

    class Renderer
    {
    public:
        Renderer(const Window* window, EventBus& eventBus);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        bool BeginFrame();
        void EndFrame();

        void SubmitTransfer(const vk::SubmitInfo& submission) const;
        U32 GetTransferQueueFamilyIdx() const { return m_TransferQueueFamilyIdx; }
        vk::CommandBuffer GetTransferCommandBufferRaw() const;

    private:
        void InitVulkan(GLFWwindow* window);
        void ShutdownVulkan();

    private:
        vk::Instance m_Instance;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        vk::SurfaceKHR m_Surface;
        vk::PhysicalDevice m_PhysicalDevice;
        vkb::Device m_VkbDevice;
        vk::Device m_Device;
        vk::Queue m_GraphicsQueue;
        vk::Queue m_PresentQueue;
        vk::Queue m_TransferQueue;

        vk::CommandPool m_GraphicsCommandPool;
        CommandBuffer m_GraphicsCommandbuffer;

        vk::CommandPool m_TransferCommandPool;
        CommandBuffer m_TransferCommandBuffer;
        vk::Fence m_TransferFence;

        vk::Fence m_RenderFence;

        Unique<Swapchain> m_Swapchain;
        vk::Image m_CurrentSwapchainImage;

        Shared<Image> m_TargetImage;

        EventConnection<WindowResizedEvent> m_WindowResizedConnection;

        U32 m_GraphicsQueueFamilyIdx = ~0;
        U32 m_PresentQueueFamilyIdx = ~0;
        U32 m_TransferQueueFamilyIdx = ~0;

        U32 m_CurrenSwapchainImageIdx = ~0;
    };
} // namespace Mango
