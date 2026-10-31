#pragma once
#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Mango/ImGui/ImGuiContext.hpp"

namespace Mango::Driver::Vulkan {
    class VkImGuiContext : public ImGuiContext
    {
    public:
        VkImGuiContext(Window* window, VkRenderer* renderer);
        ~VkImGuiContext() override;

        VkImGuiContext(const VkImGuiContext&) = delete;
        VkImGuiContext& operator=(const VkImGuiContext&) = delete;

        VkImGuiContext(VkImGuiContext&&) = delete;
        VkImGuiContext& operator=(VkImGuiContext&&) = delete;

        void BeginFrame() override;
        void EndFrame() override;

    private:
        VkRenderer* m_Renderer = nullptr;

        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    };
} // namespace Mango::Driver::Vulkan
