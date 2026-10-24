#include "Swapchain.hpp"

#include "Mango/Core/Core.hpp"

namespace Mango {
    Swapchain::Swapchain(const vkb::Device& d, U32 width, U32 height, vk::SurfaceFormatKHR format) : m_Device(d), m_Format(format)
    {
        vkb::SwapchainBuilder builder{d};
        vkb::Result<vkb::Swapchain> res = builder.set_desired_extent(width, height)
                                                  .set_desired_format(format)
                                                  .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                                                  .build();
        ASSERT(res.has_value(), std::string("Failed to create swapchain: " + res.error().message()));
        m_Swapchain = res.value();

        GetImagesAndViews();

        vk::SemaphoreCreateInfo semaphoreInfo;
        vk::ResultValue<vk::Semaphore> semaphore = m_Device.createSemaphore(semaphoreInfo);
        ASSERT(semaphore.result == vk::Result::eSuccess, "Failed to create semaphore");

        m_ImageAvailable = semaphore.value;

        for (U32 i = 0; i < m_Swapchain.image_count; i++)
        {
            semaphore = m_Device.createSemaphore(semaphoreInfo);
            ASSERT(semaphore.result == vk::Result::eSuccess, "Failed to create semaphore");
            m_RenderFinished.push_back(semaphore.value);
        }
    }

    Swapchain::~Swapchain()
    {
        m_Device.destroySemaphore(m_ImageAvailable);

        for (U32 i = 0; i < m_Swapchain.image_count; i++)
        {
            m_Device.destroySemaphore(m_RenderFinished[i]);
            m_Device.destroyImageView(m_ImageViews[i]);
        }

        m_ImageViews.clear();

        vkb::destroy_swapchain(m_Swapchain);
    }

    std::tuple<bool, vk::Image, U32> Swapchain::NextImage() const
    {
        vk::ResultValue<U32> imageIdx =
                m_Device.acquireNextImageKHR(m_Swapchain.swapchain, std::numeric_limits<U64>::max(), m_ImageAvailable);

        if (imageIdx.result == vk::Result::eErrorOutOfDateKHR)
        {
            return {false, {}, 0};
        }

        return {true, m_Images[imageIdx.value], imageIdx.value};
    }

    void Swapchain::Recreate(const vkb::Device& d, U32 width, U32 height)
    {
        vkb::SwapchainBuilder builder{d};
        vkb::Result<vkb::Swapchain> temp = builder.set_old_swapchain(m_Swapchain)
                                                   .set_desired_extent(width, height)
                                                   .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                                                   .build();
        ASSERT(temp.vk_result() == VK_SUCCESS, "Failed to recreate swapchain");
        std::swap(m_Swapchain, temp.value());
        vkb::destroy_swapchain(temp.value());

        GetImagesAndViews();
    }

    void Swapchain::GetImagesAndViews()
    {
        for (auto imageView: m_ImageViews)
            m_Device.destroyImageView(imageView);

        m_Images.clear();
        m_ImageViews.clear();

        vk::ResultValue<std::vector<vk::Image>> images = m_Device.getSwapchainImagesKHR(m_Swapchain.swapchain);
        ASSERT(images.result == vk::Result::eSuccess, "Failed to get swapchain images");
        m_Images = images.value;

        vk::ImageViewCreateInfo viewInfo;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = m_Format.format;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.levelCount = 1;

        for (auto image: m_Images)
        {
            viewInfo.image = image;
            vk::ResultValue<vk::ImageView> view = m_Device.createImageView(viewInfo);
            ASSERT(view.result == vk::Result::eSuccess, "Failed to create image view");

            m_ImageViews.push_back(view.value);
        }
    }
} // namespace Mango
