#include "VkShaderCompiler.hpp"

#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Core/Logger.hpp"

#include <filesystem>
#include <fstream>
#include <shaderc/shaderc.hpp>

namespace Mango::Driver::Vulkan {
    VkShaderCompiler::VkShaderCompiler(VkRenderer* renderer, std::string pipelineCache, std::string outputDir) :
        m_Renderer(renderer), m_PipelineCachePath(std::move(pipelineCache)), m_OutputDir(std::move(outputDir))
    {
        if (!std::filesystem::exists(m_PipelineCachePath))
        {
            std::ofstream fileCreate(m_PipelineCachePath, std::ios::binary);
        }

        std::ifstream file(m_PipelineCachePath, std::ios::ate | std::ios::binary);
        ASSERT(file.is_open(), "Failed to open file");
        std::streamsize fileSize = file.tellg();
        file.seekg(0);

        std::vector<char> data(fileSize);
        file.read(data.data(), fileSize);

        vk::PipelineCacheCreateInfo cacheInfo;
        cacheInfo.initialDataSize = data.size();
        cacheInfo.pInitialData = data.data();

        vk::Device d = m_Renderer->GetDevice();
        vk::ResultValue<vk::PipelineCache> cacheRes = d.createPipelineCache(cacheInfo);
        ASSERT(cacheRes.result == vk::Result::eSuccess, "Failed to create pipeline cache");
        m_PipelineCache = cacheRes.value;
    }

    VkShaderCompiler::~VkShaderCompiler()
    {
        if (!m_PipelineCache)
            return;

        vk::Device d = m_Renderer->GetDevice();
        vk::ResultValue<std::vector<U8>> dataRes = d.getPipelineCacheData(m_PipelineCache);
        ASSERT(dataRes.result == vk::Result::eSuccess, "Failed to get pipeline cache data");
        auto* data = reinterpret_cast<char*>(dataRes.value.data());
        std::streamsize dataSize = static_cast<std::streamsize>(dataRes.value.size());

        std::ofstream file(m_PipelineCachePath, std::ios::trunc | std::ios::binary);
        ASSERT(file.is_open(), "Failed to open file");

        file.write(data, dataSize);

        d.destroyPipelineCache(m_PipelineCache);
    }

    VkShaderModulePair VkShaderCompiler::CreateShaderModules(const std::string& vertShader, const std::string& fragShader) const
    {
        std::vector<U32> vertSpv = Compile(vertShader, shaderc_vertex_shader);
        ASSERT(!vertSpv.empty(), "Shader SPIR-V is empty");
        ASSERT(vertSpv[0] == 0x07230203, "Invalid SPIR-V");

        std::vector<U32> fragSpv = Compile(fragShader, shaderc_fragment_shader);
        ASSERT(!fragSpv.empty(), "Shader SPIR-V is empty");
        ASSERT(fragSpv[0] == 0x07230203, "Invalid SPIR-V");

        return {.vertShaderModule = CreateModule(vertSpv), .fragShaderModule = CreateModule(fragSpv)};
    }

    std::vector<U32> VkShaderCompiler::Compile(const std::string& shader, shaderc_shader_kind kind)
    {
        std::ifstream file(shader, std::ios::binary);
        ASSERT(file.is_open(), "Failed to open file");
        std::string source = {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

        if (source.size() >= 3 && static_cast<uint8_t>(source[0]) == 0xEF && static_cast<uint8_t>(source[1]) == 0xBB &&
            static_cast<uint8_t>(source[2]) == 0xBF) // UTF-8 bomb
        {
            source.erase(0, 3);
        }

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, shader.c_str(), "main", options);
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            Logger::Error("{}", result.GetErrorMessage());
            ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, "Failed to compile shader");
        }

        // TODO: Write file into output dir

        return {result.begin(), result.end()};
    }

    vk::ShaderModule VkShaderCompiler::CreateModule(const std::vector<U32>& spv) const
    {
        vk::ShaderModuleCreateInfo moduleInfo;
        moduleInfo.codeSize = spv.size() * sizeof(U32);
        moduleInfo.pCode = spv.data();

        vk::ResultValue<vk::ShaderModule> module = m_Renderer->GetDevice().createShaderModule(moduleInfo);
        ASSERT(module.result == vk::Result::eSuccess, "Failed to create shader module");
        return module.value;
    }
} // namespace Mango::Driver::Vulkan
