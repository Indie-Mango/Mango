#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Graphics/Texture.hpp"
#include "Mango/Graphics/UniformBuffer.hpp"
#include "Mango/Memory/Smart.hpp"

namespace Mango {
    class Material
    {
    public:
        static Shared<Material> Create(Renderer* renderer, Pipeline* pipeline);
        virtual ~Material() = default;

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;

        Material(Material&&) = delete;
        Material& operator=(Material&&) = delete;

        virtual void Update() = 0;
        virtual void Bind() = 0;

        Shared<UniformBuffer>& GetBuffer(U32 set, U32 binding);
        void SetBuffer(U32 set, U32 binding, const Shared<UniformBuffer>& buffer);
        
        Texture& GetTexture(U32 set, U32 binding);
        void SetTexture(U32 set, U32 binding, const Texture& texture);

    protected:
        Material() = default;

    protected:
        struct BufferBinding
        {
            Shared<UniformBuffer> Buffer;
            U32 Set = 0, Binding = 0;
        };

        struct TextureBinding
        {
            Texture Texture;
            U32 Set = 0, Binding = 0;
        };

        std::vector<BufferBinding> m_Buffers;
        std::vector<TextureBinding> m_Textures;
    };
} // namespace Mango
