#pragma once
#include <vulkan/vulkan.hpp>

namespace Mango {
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(vk::Device d, vk::CommandPool cmdPool);
        ~CommandBuffer();

        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        CommandBuffer(CommandBuffer&& other) noexcept;

        CommandBuffer& operator=(CommandBuffer&& other) noexcept;

        void Begin();
        void End();

        void BeginRendering(vk::ImageView imageView, vk::Extent2D extent);
        void EndRendering();

        vk::CommandBuffer Raw() const { return m_Buffer; }

    private:
        vk::Device m_Device = nullptr;
        vk::CommandPool m_CommandPool = nullptr;
        vk::CommandBuffer m_Buffer = nullptr;
    };
} // namespace Mango
