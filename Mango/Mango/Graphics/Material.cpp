#include "Material.hpp"

#include "Driver/Vulkan/Graphics/VkMaterial.hpp"
#include "Mango/Debug/Profile.hpp"
#include "Mango/Graphics/UniformBuffer.hpp"

namespace Mango {
    // TODO: Different constructing system
    Shared<Material> Material::Create(Renderer* renderer, PipelineHandle pipeline)
    {
        MANGO_PROFILE_SCOPE()

        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return Shared<Driver::Vulkan::VkMaterial>::Create(reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer), pipeline);
        }

        Logger::Error("Renderer uses invalid or unsupported API");
        MANGO_EXIT_PROGRAM();
    }

    Shared<UniformBuffer>& Material::GetUniformBuffer(U32 set, U32 binding)
    {
        if (auto it = std::ranges::find_if(m_Buffers,
                                           [set, binding](const BufferBinding& bb) { return bb.Set == set && bb.Binding == binding; });
            it != m_Buffers.end())
        {
            return it->Buffer;
        }

        Logger::Error("Buffer does not exist");
        MANGO_EXIT_PROGRAM();
    }

    void Material::SetUniformBuffer(U32 set, U32 binding, const Shared<UniformBuffer>& buffer)
    {
        if (auto it = std::ranges::find_if(m_Buffers,
                                           [set, binding](const BufferBinding& bb) { return bb.Set == set && bb.Binding == binding; });
            it != m_Buffers.end())
        {
            it->Buffer = buffer;
            return;
        }

        m_Buffers.emplace_back(BufferBinding{.Buffer = buffer, .Set = set, .Binding = binding});
    }

    Texture& Material::GetTexture(U32 set, U32 binding)
    {
        if (auto it = std::ranges::find_if(
                    m_Textures, [set, binding](const TextureBinding& tb) { return tb.Set == set && tb.Binding == binding; });
            it != m_Textures.end())
        {
            return it->Texture;
        }

        Logger::Error("Texture does not exist");
        MANGO_EXIT_PROGRAM();
    }

    void Material::SetTexture(U32 set, U32 binding, const Texture& texture)
    {
        if (auto it = std::ranges::find_if(
                    m_Textures, [set, binding](const TextureBinding& tb) { return tb.Set == set && tb.Binding == binding; });
            it != m_Textures.end())
        {
            it->Texture = texture;
            return;
        }

        m_Textures.emplace_back(TextureBinding{.Texture = texture, .Set = set, .Binding = binding});
    }
} // namespace Mango
