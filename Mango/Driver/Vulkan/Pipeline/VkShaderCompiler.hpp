#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Core/Filesystem.hpp"

#include <shaderc/shaderc.h>
#include <vulkan/vulkan.hpp>

namespace Mango::Driver::Vulkan {
    class VkRenderer;

    struct VkShaderModulePair
    {
        vk::ShaderModule VertShaderModule;
        vk::ShaderModule FragShaderModule;
    };

    class VkShaderCompiler
    {
    public:
        // TODO: Filesystem handle
        VkShaderCompiler(VkRenderer* renderer, std::string pipelineCache, std::string outputDir);
        ~VkShaderCompiler();

        VkShaderCompiler(const VkShaderCompiler&) = delete;
        VkShaderCompiler& operator=(const VkShaderCompiler&) = delete;

        VkShaderCompiler(VkShaderCompiler&&) = delete;
        VkShaderCompiler& operator=(VkShaderCompiler&&) = delete;

        VkShaderModulePair CreateShaderModules(const File& vertShader, const File& fragShader) const;

        vk::PipelineCache GetPipelineCache() const { return m_PipelineCache; }

    private:
        // TODO: Filesystem
        static std::vector<U32> Compile(const File& shader, shaderc_shader_kind kind);
        vk::ShaderModule CreateModule(const std::vector<U32>& spv) const;

    private:
        VkRenderer* m_Renderer = nullptr;
        std::string m_PipelineCachePath;
        std::string m_OutputDir;

        vk::PipelineCache m_PipelineCache;
    };
} // namespace Mango::Driver::Vulkan
