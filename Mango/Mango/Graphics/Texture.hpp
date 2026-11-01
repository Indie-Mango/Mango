#pragma once
#include "Image.hpp"
#include "Mango/Core/Filesystem.hpp"

namespace Mango {
    class Texture
    {
    public:
        Texture() = default;
        Texture(Renderer* renderer, const Path& path);
        ~Texture() = default;

        Texture(const Texture& other) = default;
        Texture& operator=(const Texture& other) = default;

        Texture(Texture&& other) = default;
        Texture& operator=(Texture&& other) = default;

        bool HasImage() const { return m_Image.Raw(); }
        Shared<Image>& GetImage() { return m_Image; }

        U32 GetWidth() const { return m_Image->GetWidth(); }
        U32 GetHeight() const { return m_Image->GetHeight(); }

    private:
        Shared<Image> m_Image;
    };
} // namespace Mango
