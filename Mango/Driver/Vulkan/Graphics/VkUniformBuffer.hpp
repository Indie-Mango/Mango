#pragma once
#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Mango/Graphics/UniformBuffer.hpp"

#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkUniformBuffer : public UniformBuffer
    {
    public:
        VkUniformBuffer(VkRenderer* renderer, USize size);
        ~VkUniformBuffer() override;

        VkUniformBuffer(const VkUniformBuffer&) = delete;
        VkUniformBuffer& operator=(const VkUniformBuffer&) = delete;

        VkUniformBuffer(VkUniformBuffer&&) = delete;
        VkUniformBuffer& operator=(VkUniformBuffer&&) = delete;

        void Write(const void* data, USize size) override;
        USize Size() override { return m_Size; }

        vk::Buffer GetBuffer() const { return m_Buffer; }

    private:
        VkRenderer* m_Renderer = nullptr;

        USize m_Size = 0;
        vk::Buffer m_Buffer{};
        vk::DeviceMemory m_Memory{};
    };
} // namespace Mango::Driver::Vulkan
