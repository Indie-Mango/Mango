#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Mango {
    Texture::Texture(Renderer* renderer, const Path& path)
    {
        int w, h, c;
        U8* pixels = stbi_load(path.string().c_str(), &w, &h, &c, STBI_rgb_alpha);

        m_Image = Image::Create(renderer, w, h, ImageFormat::RGBA8Srgb, ImageType::Color, ImageUsage::Texture, pixels);

        stbi_image_free(pixels);
    }
} // namespace Mango
