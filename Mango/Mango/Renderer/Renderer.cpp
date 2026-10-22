#include "Renderer.hpp"

#include "Mango/Core/Core.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <VkBootstrap.h>
#include <iostream>

namespace Mango {
    Renderer::Renderer(Window* window) { InitVulkan(window->GetNativeWindow()); }

    Renderer::~Renderer() { ShutdownVulkan(); }

    void Renderer::InitVulkan(GLFWwindow* window)
    {
        vkb::InstanceBuilder builder;
        auto instanceRes = builder.set_app_name("Mango").request_validation_layers().use_default_debug_messenger().build();
        ASSERT(instanceRes, "Failed to create instance");

        vkb::Instance vkbInstance = instanceRes.value();
        m_Instance = vkbInstance.instance;
        m_DebugMessenger = vkbInstance.debug_messenger;

        VkSurfaceKHR rawSurface = nullptr;

        if (VkResult _ = glfwCreateWindowSurface(m_Instance, window, nullptr, &rawSurface))
        {
            const char* msg;
            int res = glfwGetError(&msg);
            if (res != 0)
            {
                std::cout << res << " ";
                if (msg != nullptr)
                    std::cout << msg;
                std::cout << "\n";
            }

            ASSERT(false, "");
        }

        m_Surface = rawSurface;

        vkb::PhysicalDeviceSelector selector{vkbInstance};
        auto physRes = selector.set_surface(rawSurface).set_minimum_version(1, 4).require_dedicated_transfer_queue().select();
        ASSERT(physRes, physRes.error().message());

        vkb::PhysicalDevice vkbPd = physRes.value();
        m_PhysicalDevice = vkbPd.physical_device;

        vkb::DeviceBuilder deviceBuilder{vkbPd};
        auto devRes = deviceBuilder.build();
        ASSERT(devRes, "Failed to build device");

        vkb::Device vkbDevice = devRes.value();
        m_Device = vkbDevice.device;

        auto graphicsQueueRes = vkbDevice.get_queue(vkb::QueueType::graphics);
        ASSERT(graphicsQueueRes, "Failed to get graphics queue");
        m_GraphicsQueue = graphicsQueueRes.value();

        auto presentQueueRes = vkbDevice.get_queue(vkb::QueueType::present);
        ASSERT(presentQueueRes, "Failed to get graphics queue");
        m_PresentQueue = graphicsQueueRes.value();

        auto transferQueueRes = vkbDevice.get_queue(vkb::QueueType::transfer);
        ASSERT(transferQueueRes, "Failed to get graphics queue");
        m_TransferQueue = transferQueueRes.value();
    }

    void Renderer::ShutdownVulkan() const
    {
        vk::Result waitRes =m_Device.waitIdle();
        ASSERT(waitRes == vk::Result::eSuccess, "Failed to wait for device idle");

        m_Device.destroy();
        m_Instance.destroySurfaceKHR(m_Surface);
        vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
        m_Instance.destroy();
    }
} // namespace Mango
