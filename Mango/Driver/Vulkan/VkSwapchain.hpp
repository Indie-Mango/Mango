#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkSwapchain
    {
    public:
        VkSwapchain(const vkb::Device& d, U32 width, U32 height, vk::SurfaceFormatKHR format);
        ~VkSwapchain();

        VkSwapchain(const VkSwapchain&) = delete;
        VkSwapchain& operator=(const VkSwapchain&) = delete;

        VkSwapchain(VkSwapchain&&) = delete;
        VkSwapchain& operator=(VkSwapchain&&) = delete;

        vk::SwapchainKHR Raw() const { return m_Swapchain.swapchain; }

        std::tuple<bool, vk::Image, U32> NextImage() const;
        void Recreate(const vkb::Device& d, U32 width, U32 height);

        vk::SurfaceFormatKHR GetFormat() const { return m_Format; }

        vk::Semaphore IsImageAvailable() const { return m_ImageAvailable; }
        vk::Semaphore IsRenderFinished(U32 idx) const { return m_RenderFinished[idx]; }

        U32 GetWidth() const { return m_Width; }
        U32 GetHeight() const { return m_Height; }

    private:
        void GetImagesAndViews();

    private:
        vk::Device m_Device;

        vkb::Swapchain m_Swapchain;
        vk::SurfaceFormatKHR m_Format;

        std::vector<vk::Image> m_Images;
        std::vector<vk::ImageView> m_ImageViews;

        vk::Semaphore m_ImageAvailable;
        std::vector<vk::Semaphore> m_RenderFinished;

        U32 m_Width = 0, m_Height = 0;
    };
} // namespace Mango::Driver::Vulkan
