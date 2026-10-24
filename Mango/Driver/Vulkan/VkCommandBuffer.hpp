#pragma once
#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkCommandBuffer
    {
    public:
        VkCommandBuffer() = default;
        VkCommandBuffer(vk::Device d, vk::CommandPool cmdPool);
        ~VkCommandBuffer();

        VkCommandBuffer(const VkCommandBuffer&) = delete;
        VkCommandBuffer& operator=(const VkCommandBuffer&) = delete;

        VkCommandBuffer(VkCommandBuffer&& other) noexcept;

        VkCommandBuffer& operator=(VkCommandBuffer&& other) noexcept;

        void Begin() const;
        void End() const;

        void BeginRendering(vk::ImageView imageView, vk::Extent2D extent) const;
        void EndRendering() const;

        vk::CommandBuffer Raw() const { return m_Buffer; }

    private:
        vk::Device m_Device = nullptr;
        vk::CommandPool m_CommandPool = nullptr;
        vk::CommandBuffer m_Buffer = nullptr;
    };
}
