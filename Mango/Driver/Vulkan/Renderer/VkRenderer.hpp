#pragma once
#include "Driver/Vulkan/Pipeline/VkShaderCompiler.hpp"
#include "Driver/Vulkan/VkCommandBuffer.hpp"
#include "Driver/Vulkan/VkSwapchain.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Renderer/Renderer.hpp"

#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkRenderer : public Renderer
    {
    public:
        VkRenderer(const Window* window, EventBus& eventBus);
        ~VkRenderer() override;

        VkRenderer(const VkRenderer&) = delete;
        VkRenderer& operator=(const VkRenderer&) = delete;

        VkRenderer(VkRenderer&&) = delete;
        VkRenderer& operator=(VkRenderer&&) = delete;

        bool BeginFrame() override;
        void EndFrame() override;

        void WaitIdle() override
        {
            vk::Result waitRes = m_Device.waitIdle();
            ASSERT(waitRes == vk::Result::eSuccess, "Failed to wait for device idle");
        }

        void OnWindowResize(const Window* window) const;

        vk::PhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        vk::Device GetDevice() const { return m_Device; }

        void SubmitTransfer() const;
        U32 GetTransferQueueFamilyIdx() const { return m_TransferQueueFamilyIdx; }
        vk::CommandBuffer GetTransferCommandBufferRaw() const;

        vk::SurfaceFormatKHR GetSurfaceFormat() const { return m_Swapchain->GetFormat(); }

        VkShaderCompiler* GetShaderCompiler() const { return m_ShaderCompiler.Raw(); }

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
        VkCommandBuffer m_GraphicsCommandBuffer;

        vk::CommandPool m_TransferCommandPool;
        VkCommandBuffer m_TransferCommandBuffer;
        vk::Fence m_TransferFence;

        vk::Fence m_RenderFence;

        Unique<VkSwapchain> m_Swapchain;
        vk::Image m_CurrentSwapchainImage;

        Unique<VkShaderCompiler> m_ShaderCompiler;

        U32 m_GraphicsQueueFamilyIdx = ~0;
        U32 m_PresentQueueFamilyIdx = ~0;
        U32 m_TransferQueueFamilyIdx = ~0;

        U32 m_CurrenSwapchainImageIdx = ~0;
    };
} // namespace Mango::Driver::Vulkan
