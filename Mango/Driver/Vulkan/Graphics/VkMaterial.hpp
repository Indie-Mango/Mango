#pragma once
#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Mango/Graphics/Material.hpp"
#include "Mango/Graphics/PipelineManager.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkMaterial : public Material
    {
    public:
        VkMaterial(VkRenderer* renderer, PipelineHandle pipeline);
        ~VkMaterial() override = default;

        VkMaterial(const VkMaterial& other) = delete;
        VkMaterial& operator=(const VkMaterial& other) = delete;

        VkMaterial(VkMaterial&& other) = delete;
        VkMaterial& operator=(VkMaterial&& other) = delete;

        void Update() override;
        void Bind() override;

    private:
        VkRenderer* m_Renderer = nullptr;
        PipelineHandle m_Pipeline = 0;

        std::vector<vk::DescriptorSet> m_Sets;
    };
} // namespace Mango::Driver::Vulkan
