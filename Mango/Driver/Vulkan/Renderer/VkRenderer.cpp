#include "VkRenderer.hpp"

#include "Driver/Vulkan/Graphics/VkImage.hpp"
#include "Driver/Vulkan/Graphics/VkIndexBuffer.hpp"
#include "Driver/Vulkan/Graphics/VkVertexBuffer.hpp"
#include "Driver/Vulkan/Pipeline/VkPipeline.hpp"
#include "Driver/Vulkan/VkUtils.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Core/Logger.hpp"
#include "Mango/Graphics/Image.hpp"

#include <GLFW/glfw3.h>

namespace Mango::Driver::Vulkan {
    VkRenderer::VkRenderer(const Window* window, EventBus& eventBus) : Renderer(eventBus)
    {
        InitVulkan(window->GetNativeWindow());

        auto available = m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface).value;
        vk::SurfaceFormatKHR surfaceFormat = VkUtils::PickSurfaceFormat(available);
        m_Swapchain = Unique<VkSwapchain>::Create(m_VkbDevice, window->GetWidth(), window->GetHeight(), surfaceFormat);

        m_ShaderCompiler = Unique<VkShaderCompiler>::Create(this, "Mango-Cache/Pipeline.cache", "Mango-Cache/Shaders");

        m_TargetImage =
                Image::Create(this, window->GetWidth(), window->GetHeight(), ImageType::Color, ImageUsage::ColorAttachment, nullptr);
    }

    VkRenderer::~VkRenderer()
    {
        vk::Result waitRes = m_Device.waitIdle();
        ASSERT(waitRes == vk::Result::eSuccess, "Failed to wait for device idle");

        m_TargetImage.Reset();

        ShutdownVulkan();
    }

    bool VkRenderer::BeginFrame()
    {
        vk::Result fenceRes = m_Device.waitForFences(m_RenderFence, vk::True, std::numeric_limits<U64>::max());
        ASSERT(fenceRes == vk::Result::eSuccess, "Failed to wait for fence");

        bool validSwapchain = false;
        std::tie(validSwapchain, m_CurrentSwapchainImage, m_CurrenSwapchainImageIdx) = m_Swapchain->NextImage();
        if (!validSwapchain)
        {
            return false;
        }

        m_GraphicsCommandBuffer.Begin();

        auto* vkTargetImage = reinterpret_cast<VkImage*>(m_TargetImage.Raw());
        vkTargetImage->TransitionLayout(m_GraphicsCommandBuffer.Raw(), vk::ImageLayout::eUndefined,
                                        vk::ImageLayout::eColorAttachmentOptimal, vk::PipelineStageFlagBits2::eNone,
                                        vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::AccessFlagBits2::eNone,
                                        vk::AccessFlagBits2::eColorAttachmentWrite);

        m_GraphicsCommandBuffer.BeginRendering(vkTargetImage->GetView(), {m_TargetImage->GetWidth(), m_TargetImage->GetHeight()});
        m_Rendering = true;

        vk::Viewport viewport;
        viewport.width = static_cast<F32>(m_TargetImage->GetWidth());
        viewport.height = static_cast<F32>(m_TargetImage->GetHeight());
        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;
        m_GraphicsCommandBuffer.Raw().setViewport(0, viewport);

        vk::Rect2D scissor;
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = vk::Extent2D{m_TargetImage->GetWidth(), m_TargetImage->GetHeight()};
        m_GraphicsCommandBuffer.Raw().setScissor(0, scissor);

        return true;
    }

    void VkRenderer::EndFrame()
    {
        m_Rendering = false;
        m_GraphicsCommandBuffer.EndRendering();

        auto* vkTargetImage = reinterpret_cast<VkImage*>(m_TargetImage.Raw());
        vkTargetImage->TransitionLayout(m_GraphicsCommandBuffer.Raw(), vk::ImageLayout::eColorAttachmentOptimal,
                                        vk::ImageLayout::eTransferSrcOptimal, vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                                        vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eColorAttachmentWrite,
                                        vk::AccessFlagBits2::eTransferRead);

        VkUtils::TransitionImageLayout(m_GraphicsCommandBuffer.Raw(), vk::ImageLayout::eUndefined,
                                       vk::ImageLayout::eTransferDstOptimal, vk::PipelineStageFlagBits2::eNone,
                                       vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eNone,
                                       vk::AccessFlagBits2::eTransferWrite, vk::ImageAspectFlagBits::eColor, m_CurrentSwapchainImage);

        vk::ImageCopy region;
        region.srcSubresource.layerCount = 1;
        region.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.dstSubresource.layerCount = 1;
        region.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.extent = vk::Extent3D{m_TargetImage->GetWidth(), m_TargetImage->GetHeight(), 1};
        m_GraphicsCommandBuffer.Raw().copyImage(vkTargetImage->GetImage(), vk::ImageLayout::eTransferSrcOptimal,
                                                m_CurrentSwapchainImage, vk::ImageLayout::eTransferDstOptimal, region);

        VkUtils::TransitionImageLayout(m_GraphicsCommandBuffer.Raw(), vk::ImageLayout::eTransferDstOptimal,
                                       vk::ImageLayout::ePresentSrcKHR, vk::PipelineStageFlagBits2::eTransfer,
                                       vk::PipelineStageFlagBits2::eNone, vk::AccessFlagBits2::eTransferWrite,
                                       vk::AccessFlagBits2::eNone, vk::ImageAspectFlagBits::eColor, m_CurrentSwapchainImage);

        m_GraphicsCommandBuffer.End();

        vk::CommandBuffer cmd = m_GraphicsCommandBuffer.Raw();

        vk::Semaphore imageAvailable = m_Swapchain->IsImageAvailable();

        vk::Semaphore renderFinished = m_Swapchain->IsRenderFinished(m_CurrenSwapchainImageIdx);

        vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eTransfer;

        vk::SubmitInfo submission{};
        submission.waitSemaphoreCount = 1;
        submission.pWaitSemaphores = &imageAvailable;
        submission.pWaitDstStageMask = &waitStage;

        submission.commandBufferCount = 1;
        submission.pCommandBuffers = &cmd;

        submission.signalSemaphoreCount = 1;
        submission.pSignalSemaphores = &renderFinished;

        vk::Result fenceRes = m_Device.resetFences(m_RenderFence);
        ASSERT(fenceRes == vk::Result::eSuccess, "Failed to reset fence");

        vk::Result res = m_GraphicsQueue.submit(submission, m_RenderFence);
        ASSERT(res == vk::Result::eSuccess, "Failed to submit graphics queue");

        // Present
        vk::SwapchainKHR swapchain = m_Swapchain->Raw();

        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &m_CurrenSwapchainImageIdx;

        vk::Result presentResult = m_PresentQueue.presentKHR(presentInfo);
        ASSERT(presentResult == vk::Result::eSuccess || presentResult == vk::Result::eSuboptimalKHR,
               "Failed to present swapchain image");
    }

    void VkRenderer::DrawIndexed(U32 indexCount)
    {
        ASSERT(m_Rendering, "To draw, the renderer must be rendering");

        m_GraphicsCommandBuffer.Raw().drawIndexed(indexCount, 1, 0, 0, 0);
    }

    void VkRenderer::SetVertexBuffer(const Shared<VertexBuffer>& vertexBuffer)
    {
        ASSERT(m_Rendering, "To set a vertex buffer, the renderer must be rendering");

        vk::Buffer buffer = reinterpret_cast<VkVertexBuffer*>(vertexBuffer.Raw())->GetBuffer();
        m_GraphicsCommandBuffer.Raw().bindVertexBuffers(0u, buffer, {0u});
    }

    void VkRenderer::SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer)
    {
        ASSERT(m_Rendering, "To set a index buffer, the renderer must be rendering");

        vk::Buffer buffer = reinterpret_cast<VkIndexBuffer*>(indexBuffer.Raw())->GetBuffer();
        m_GraphicsCommandBuffer.Raw().bindIndexBuffer(buffer, 0, vk::IndexType::eUint32);
    }

    void VkRenderer::SetPipeline(const Shared<Pipeline>& pipeline)
    {
        ASSERT(m_Rendering, "To set the pipeline, the renderer must be rendering");

        vk::Pipeline raw = reinterpret_cast<VkPipeline*>(pipeline.Raw())->GetPipeline();
        m_GraphicsCommandBuffer.Raw().bindPipeline(vk::PipelineBindPoint::eGraphics, raw);
    }

    void VkRenderer::OnWindowResizeVk(const Window* window)
    {
        if (window->GetWidth() == 0 || window->GetHeight() == 0)
            return;

        WaitIdle();

        m_Swapchain->Recreate(m_VkbDevice, window->GetWidth(), window->GetHeight());
        m_TargetImage->Resize(window->GetWidth(), window->GetHeight());
    }

    void VkRenderer::SubmitTransfer() const
    {
        vk::CommandBuffer transferCommandBuffer = m_TransferCommandBuffer.Raw();

        vk::SubmitInfo submission;
        submission.commandBufferCount = 1;
        submission.pCommandBuffers = &transferCommandBuffer;

        vk::Result res = m_TransferQueue.submit(submission, m_TransferFence);
        ASSERT(res == vk::Result::eSuccess, "Failed to submit transfer command buffer to queue");
    }

    vk::CommandBuffer VkRenderer::GetTransferCommandBufferRaw() const
    {
        vk::Result waitRes = m_Device.waitForFences(m_TransferFence, vk::True, std::numeric_limits<U64>::max());
        ASSERT(waitRes == vk::Result::eSuccess, "Failed to wait for transfer operation complete");

        vk::Result resetFenceRes = m_Device.resetFences(m_TransferFence);
        ASSERT(resetFenceRes == vk::Result::eSuccess, "Failed to reset transfer fence");

        return m_TransferCommandBuffer.Raw();
    }

    void VkRenderer::InitVulkan(GLFWwindow* window)
    {
        vkb::InstanceBuilder builder;
        auto instanceRes =
                builder.set_app_name("Mango")
                        .require_api_version(1, 4, 0)
                        .request_validation_layers()
                        .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT,
                                               const VkDebugUtilsMessengerCallbackDataEXT* data, void*) -> VkBool32 {
                            switch (severity)
                            {
                                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                                    break;
                                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                                    Logger::Info("{}", data->pMessage);
                                    break;
                                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                                    Logger::Warn("{}", data->pMessage);
                                    break;
                                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                                    Logger::Error("{}", data->pMessage);
                                    break;
                                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                                    Logger::Error("{}", data->pMessage);
                                    MANGO_EXIT_PROGRAM();
                            }

                            return VK_FALSE;
                        })
                        .build();

        ASSERT(instanceRes, "Failed to create instance");

        vkb::Instance vkbInstance = instanceRes.value();
        m_Instance = vkbInstance.instance;
        m_DebugMessenger = vkbInstance.debug_messenger;

        VkSurfaceKHR rawSurface = nullptr;

        if (VkResult _ = glfwCreateWindowSurface(m_Instance, window, nullptr, &rawSurface))
        {
            const char* msg;
            if (int res = glfwGetError(&msg); res != 0)
            {
                if (msg != nullptr)
                    Logger::Error("{}", msg);
            }

            MANGO_EXIT_PROGRAM();
        }

        m_Surface = rawSurface;

        vkb::PhysicalDeviceSelector selector{vkbInstance};
        vk::PhysicalDeviceFeatures requiredFeatures10;
        requiredFeatures10.fillModeNonSolid = vk::True;
        vk::PhysicalDeviceVulkan13Features requiredFeatures13;
        requiredFeatures13.dynamicRendering = vk::True;
        requiredFeatures13.synchronization2 = vk::True;
        auto physRes = selector.set_surface(rawSurface)
                               .set_minimum_version(1, 4)
                               .set_required_features(requiredFeatures10)
                               .set_required_features_13(requiredFeatures13)
                               .require_dedicated_transfer_queue()
                               .select();
        ASSERT(physRes, "{}", physRes.error().message());

        const vkb::PhysicalDevice& vkbPd = physRes.value();
        m_PhysicalDevice = vkbPd.physical_device;

        vkb::DeviceBuilder deviceBuilder{vkbPd};
        auto devRes = deviceBuilder.build();
        ASSERT(devRes, "Failed to build device");

        m_VkbDevice = devRes.value();
        m_Device = m_VkbDevice.device;

        {
            auto graphicsQueueRes = m_VkbDevice.get_queue_and_index(vkb::QueueType::graphics);
            ASSERT(graphicsQueueRes, "Failed to get graphics queue");
            m_GraphicsQueue = graphicsQueueRes.value().first;
            m_GraphicsQueueFamilyIdx = graphicsQueueRes.value().second;
        }

        {
            auto presentQueueRes = m_VkbDevice.get_queue_and_index(vkb::QueueType::present);
            ASSERT(presentQueueRes, "Failed to get graphics queue");
            m_PresentQueue = presentQueueRes.value().first;
            m_PresentQueueFamilyIdx = presentQueueRes.value().second;
        }

        {
            auto transferQueueRes = m_VkbDevice.get_queue_and_index(vkb::QueueType::transfer);
            ASSERT(transferQueueRes, "Failed to get graphics queue");
            m_TransferQueue = transferQueueRes.value().first;
            m_TransferQueueFamilyIdx = transferQueueRes.value().second;
        }

        vk::CommandPoolCreateInfo commandPoolInfo;
        commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamilyIdx;
        vk::Result cmdPoolRes = m_Device.createCommandPool(&commandPoolInfo, nullptr, &m_GraphicsCommandPool);
        ASSERT(cmdPoolRes == vk::Result::eSuccess, "Failed to create command pool");

        m_GraphicsCommandBuffer = VkCommandBuffer{m_Device, m_GraphicsCommandPool};

        commandPoolInfo.queueFamilyIndex = m_TransferQueueFamilyIdx;
        cmdPoolRes = m_Device.createCommandPool(&commandPoolInfo, nullptr, &m_TransferCommandPool);
        ASSERT(cmdPoolRes == vk::Result::eSuccess, "Failed to create command pool");
        m_TransferCommandBuffer = VkCommandBuffer{m_Device, m_TransferCommandPool};

        vk::FenceCreateInfo fenceInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        vk::ResultValue<vk::Fence> fence = m_Device.createFence(fenceInfo);
        ASSERT(fence.result == vk::Result::eSuccess, "Failed to create fence");
        m_TransferFence = fence.value;

        fence = m_Device.createFence(fenceInfo);
        ASSERT(fence.result == vk::Result::eSuccess, "Failed to create fence");
        m_RenderFence = fence.value;
    }

    void VkRenderer::ShutdownVulkan()
    {
        m_GraphicsQueueFamilyIdx = ~0;
        m_PresentQueueFamilyIdx = ~0;
        m_TransferQueueFamilyIdx = ~0;

        m_ShaderCompiler.Reset();

        m_Swapchain.Reset();

        m_Device.destroyFence(m_RenderFence);

        m_Device.destroyFence(m_TransferFence);
        m_TransferCommandBuffer = VkCommandBuffer{};
        m_Device.destroyCommandPool(m_TransferCommandPool);

        m_GraphicsCommandBuffer = VkCommandBuffer{};
        m_Device.destroyCommandPool(m_GraphicsCommandPool);

        m_Device.destroy();
        m_Instance.destroySurfaceKHR(m_Surface);
        vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
        m_Instance.destroy();
    }
} // namespace Mango::Driver::Vulkan
