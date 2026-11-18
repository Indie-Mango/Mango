#include "Texture.hpp"

#include "Mango/Debug/Profile.hpp"
#include "Mango/Main/Engine.hpp"
#include "Mango/Resource/Resource.hpp"
#include "Mango/Resource/ResourceManager.hpp"

namespace Mango {
    Texture::Texture(Engine* engine, ResourceHandle picture)
    {
        MANGO_PROFILE_SCOPE()

        auto* rawPicture = reinterpret_cast<Picture*>(engine->GetResourceManager().ToRaw(picture));
        m_Image = Image::Create(engine->GetRenderer(), rawPicture->Width, rawPicture->Height, ImageFormat::RGBA8Srgb, ImageType::Color,
                                ImageUsage::Texture, rawPicture->Data);
    }
} // namespace Mango
