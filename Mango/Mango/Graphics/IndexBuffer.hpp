#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Mango {
    class IndexBuffer
    {
    public:
        IndexBuffer(vk::PhysicalDevice pd, vk::Device d, vk::CommandBuffer transferCommandBuffer, const std::vector<U32>& indices);

        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;

        IndexBuffer(IndexBuffer&&) = delete;
        IndexBuffer& operator=(IndexBuffer&&) = delete;

        void SetData(const std::vector<U32>& data);

    private:
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_Device;
        vk::CommandBuffer m_TransferCommandBuffer;

        vk::Buffer m_Buffer;
        vk::DeviceMemory m_Memory;

        vk::Buffer m_StagingBuffer;
        vk::DeviceMemory m_StagingBufferMemory;

        USize m_IndexCount = 0;
        USize m_Size = 0;
    };
} // namespace Mango
