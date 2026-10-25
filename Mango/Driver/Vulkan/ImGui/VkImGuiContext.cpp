#include "VkImGuiContext.hpp"

#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Mango/Base/Window.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace Mango::Driver::Vulkan {
    VkImGuiContext::VkImGuiContext(Window* window, VkRenderer* renderer) : m_Renderer(renderer)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.95f);

        ImGui_ImplGlfw_InitForVulkan(window->GetNativeWindow(), true);

        ImGui_ImplVulkan_InitInfo vkInitInfo = {};
        vkInitInfo.Instance = renderer->GetInstance();
        vkInitInfo.PhysicalDevice = renderer->GetPhysicalDevice();
        vkInitInfo.Device = renderer->GetDevice();
        vkInitInfo.QueueFamily = renderer->GetGraphicsQueueFamilyIdx();
        vkInitInfo.Queue = renderer->GetGraphicsQueue();
        vkInitInfo.PipelineCache = renderer->GetShaderCompiler()->GetPipelineCache();

        VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                             {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                             {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000}};

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 3000;
        pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
        pool_info.pPoolSizes = pool_sizes;

        VkResult result = vkCreateDescriptorPool(renderer->GetDevice(), &pool_info, nullptr, &m_DescriptorPool);

        ASSERT(result == VK_SUCCESS, "Failed to create ImGui descriptor pool");

        vkInitInfo.DescriptorPool = m_DescriptorPool;
        vkInitInfo.MinImageCount = 2;
        vkInitInfo.ImageCount = renderer->GetSwapchain()->GetImageCount();
        vkInitInfo.Allocator = nullptr;
        vkInitInfo.UseDynamicRendering = true;

        vkInitInfo.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
        vkInitInfo.PipelineInfoMain.Subpass = 0;
        vkInitInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        auto colorFormat = static_cast<VkFormat>(renderer->GetSurfaceFormat().format);

        vkInitInfo.PipelineInfoMain.PipelineRenderingCreateInfo = {};
        vkInitInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

        vkInitInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        vkInitInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &colorFormat;
        ImGui_ImplVulkan_Init(&vkInitInfo);
    }

    VkImGuiContext::~VkImGuiContext()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        m_Renderer->GetDevice().destroyDescriptorPool(m_DescriptorPool);
    }

    void VkImGuiContext::BeginFrame()
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();
    }

    void VkImGuiContext::EndFrame()
    {
        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(drawData, m_Renderer->GetGraphicsCommandBufferRaw());
    }
} // namespace Mango::Driver::Vulkan
