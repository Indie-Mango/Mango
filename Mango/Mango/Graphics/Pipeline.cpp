#include "Pipeline.hpp"

#include "Driver/Vulkan/Pipeline/VkPipeline.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Debug/Profile.hpp"
#include "Mango/Renderer/Renderer.hpp"

namespace Mango {
    Shared<Pipeline> Pipeline::Create(Renderer* renderer, const PipelineInfo& pipelineInfo, const File& vertShader,
                                      const File& fragShader)
    {
        MANGO_PROFILE_SCOPE()
        
        switch (Renderer::GetAPI())
        {
            case Renderer::API::Vulkan:
                return Shared<Driver::Vulkan::VkPipeline>::Create(reinterpret_cast<Driver::Vulkan::VkRenderer*>(renderer),
                                                                  pipelineInfo, vertShader, fragShader);
        }

        Logger::Error("Renderer uses invalid or unsupported API");
        MANGO_EXIT_PROGRAM();
    }
} // namespace Mango
