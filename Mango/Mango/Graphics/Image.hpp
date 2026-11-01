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
        ColorAttachment,
        Texture
    };

    enum class ImageFormat : U8
    {
        R8Unorm,
        RG8Unorm,
        RGBA8Unorm,
        RGBA8Srgb,
        BGRA8Unorm,
        BGRA8Srgb,
        RGBA16Float,
        RGBA32Float
    };

    constexpr U32 ImageFormatBytesPerPixel(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::R8Unorm:
                return 1;
            case ImageFormat::RG8Unorm:
                return 2;
            case ImageFormat::RGBA8Unorm:
            case ImageFormat::RGBA8Srgb:
            case ImageFormat::BGRA8Unorm:
            case ImageFormat::BGRA8Srgb:
                return 4;
            case ImageFormat::RGBA16Float:
                return 8;
            case ImageFormat::RGBA32Float:
                return 16;
        }

        return 0;
    }

    // TODO: Rework
    class Image
    {
    public:
        static Shared<Image> Create(Renderer* renderer, U32 width, U32 height, ImageFormat format, ImageType type, ImageUsage usage,
                                    const void* data);
        virtual ~Image() = default;

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        Image(Image&&) = delete;
        Image& operator=(Image&&) = delete;

        virtual void Resize(U32 width, U32 height) = 0;
        virtual void SetData(const void* data) = 0;

        U32 GetWidth() const { return m_Width; }
        U32 GetHeight() const { return m_Height; }
        ImageFormat GetFormat() const { return m_Format; }

    protected:
        Image(U32 width, U32 height, ImageFormat format);

    protected:
        U32 m_Width = 0, m_Height = 0;
        ImageFormat m_Format;
    };
} // namespace Mango
