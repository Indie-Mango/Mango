#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Memory/Smart.hpp"

namespace Mango {
    class Renderer;

    enum class ImageType : U8
    {
        Color
    };

    enum class ImageUsage : U8
    {
        ColorAttachment
    };

    class Image
    {
    public:
        static Shared<Image> Create(Renderer* renderer, U32 width, U32 height, ImageType type, ImageUsage usage, const void* data);
        virtual ~Image() = default;

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        Image(Image&&) = delete;
        Image& operator=(Image&&) = delete;

        U32 GetWidth() const { return m_Width; }
        U32 GetHeight() const { return m_Height; }

    protected:
        Image(U32 width, U32 height);

    protected:
        U32 m_Width = 0, m_Height = 0;
    };
} // namespace Mango
