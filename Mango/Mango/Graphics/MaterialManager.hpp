#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Graphics/Material.hpp"

namespace Mango {
    class MaterialManager
    {
    public:
        MaterialManager() = default;
        ~MaterialManager() = default;

        MaterialManager(const MaterialManager& other) = delete;
        MaterialManager& operator=(const MaterialManager& other) = delete;

        MaterialManager(MaterialManager&& other) = delete;
        MaterialManager& operator=(MaterialManager&& other) = delete;

        template<typename... Args>
        MaterialHandle CreateMaterial(Args&&... args)
        {
            ASSERT(m_CurrentHandle < UINT64_MAX, "Cannot have 2^64 materials");

            m_Materials.emplace(m_CurrentHandle, Material::Create(std::forward<Args>(args)...));

            return m_CurrentHandle++;
        }

        void Release(MaterialHandle handle)
        {
            if (auto it = m_Materials.find(handle); it != m_Materials.end())
            {
                m_Materials.erase(it);
            }
        }

        bool IsValid(MaterialHandle handle) const { return m_Materials.contains(handle); }

        Material* ToRaw(MaterialHandle handle) const
        {
            ASSERT(IsValid(handle), "Handle must be valid");

            return m_Materials.at(handle).Raw();
        }

        Shared<UniformBuffer>& GetUniformBuffer(MaterialHandle handle, U32 set, U32 binding) const
        {
            return ToRaw(handle)->GetUniformBuffer(set, binding);
        }

        void SetUniformBuffer(MaterialHandle handle, U32 set, U32 binding, const Shared<UniformBuffer>& buffer) const
        {
            ToRaw(handle)->SetUniformBuffer(set, binding, buffer);
        }

        Texture& GetTexture(MaterialHandle handle, U32 set, U32 binding) const { return ToRaw(handle)->GetTexture(set, binding); }

        void SetTexture(MaterialHandle handle, U32 set, U32 binding, const Texture& texture) const
        {
            ToRaw(handle)->SetTexture(set, binding, texture);
        }

        void UpdateMaterials() const
        {
            for (auto& m : m_Materials | std::views::values)
            {
                m->Update();
            }
        }

        void Bind(MaterialHandle handle) const { ToRaw(handle)->Bind(); }

    private:
        std::map<MaterialHandle, Shared<Material>> m_Materials;
        U64 m_CurrentHandle = 1;
    };
} // namespace Mango
