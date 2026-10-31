#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Memory/Smart.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    class UniformBuffer
    {
    public:
        static Shared<UniformBuffer> Create(Renderer* renderer, USize size);
        virtual ~UniformBuffer() = default;

        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;

        UniformBuffer(UniformBuffer&&) = delete;
        UniformBuffer& operator=(UniformBuffer&&) = delete;

        virtual void Write(const void* data, USize size) = 0;
        virtual USize Size() = 0;

    protected:
        UniformBuffer() = default;
    };
} // namespace Mango
