#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Graphics/Pipeline.hpp"

namespace Mango {
    using PipelineHandle = U64;

    class PipelineManager
    {
    public:
        PipelineManager() = default;
        ~PipelineManager() = default;

        PipelineManager(const PipelineManager& other) = delete;
        PipelineManager& operator=(const PipelineManager& other) = delete;

        PipelineManager(PipelineManager&& other) = delete;
        PipelineManager& operator=(PipelineManager&& other) = delete;

        template<typename... Args>
        PipelineHandle CreatePipeline(Args&&... args)
        {
            ASSERT(m_CurrentHandle < UINT64_MAX, "Cannot have 2^64 pipelines");

            m_Pipelines.emplace(m_CurrentHandle, Pipeline::Create(std::forward<Args>(args)...));

            return m_CurrentHandle++;
        }

        void Release(PipelineHandle handle)
        {
            if (auto it = m_Pipelines.find(handle); it != m_Pipelines.end())
            {
                m_Pipelines.erase(it);
            }
        }

        bool IsValid(PipelineHandle handle) const { return m_Pipelines.contains(handle); }

        Pipeline* ToRaw(PipelineHandle handle) const
        {
            ASSERT(IsValid(handle), "Handle must be valid");

            return m_Pipelines.at(handle).Raw();
        }

    private:
        std::map<PipelineHandle, Shared<Pipeline>> m_Pipelines;
        PipelineHandle m_CurrentHandle = 1;
    };
} // namespace Mango
