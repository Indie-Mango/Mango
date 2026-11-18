#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Memory/Smart.hpp"
#include "Mango/Resource/Resource.hpp"

#include <map>

namespace Mango {
    class ResourceManager
    {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        ResourceManager(const ResourceManager& other) = delete;
        ResourceManager& operator=(const ResourceManager& other) = delete;

        ResourceManager(ResourceManager&& other) = delete;
        ResourceManager& operator=(ResourceManager&& other) = delete;

        template<typename T, typename... Args>
        ResourceHandle CreateResource(Args&&... args)
        {
            m_Resources.emplace(m_CurrentHandle, Unique<T>::Create(std::forward<Args>(args)...));

            return m_CurrentHandle++;
        }

        void Release(ResourceHandle handle)
        {
            if (auto it = m_Resources.find(handle); it != m_Resources.end())
            {
                UnloadResource(handle);
                m_Resources.erase(it);
            }
        }

        bool IsValid(PipelineHandle handle) const { return m_Resources.contains(handle); }

        Resource* ToRaw(PipelineHandle handle) const
        {
            ASSERT(IsValid(handle), "Handle must be valid");

            return m_Resources.at(handle).Raw();
        }

        bool LoadResource(ResourceHandle handle) const
        {
            Resource* r = ToRaw(handle);

            if (r->IsLoaded())
                r->Unload();

            return r->Load();
        }

        void UnloadResource(ResourceHandle handle) const
        {
            Resource* r = ToRaw(handle);

            if (r->IsLoaded())
                r->Unload();
        }

        bool IsResourceLoaded(ResourceHandle handle) const { return ToRaw(handle)->IsLoaded(); }

        const File& GetSource(ResourceHandle handle) const { return ToRaw(handle)->File; }

        void SetSource(ResourceHandle handle, const Path& src) const
        {
            Resource* r = ToRaw(handle);

            if (r->IsLoaded())
                r->Unload();

            r->File = File{src, FileMode::Read};
        }

    private:
        std::map<ResourceHandle, Unique<Resource>> m_Resources;
        ResourceHandle m_CurrentHandle = 1;
    };
} // namespace Mango
