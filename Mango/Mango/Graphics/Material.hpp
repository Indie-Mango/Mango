#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Memory/Smart.hpp"
#include "UniformBuffer.hpp"

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

    protected:
        Material() = default;

    protected:
        struct BufferBinding
        {
            Shared<UniformBuffer> Buffer;
            U32 Set = 0, Binding = 0;
        };

        std::vector<BufferBinding> m_Buffers;
    };
} // namespace Mango
