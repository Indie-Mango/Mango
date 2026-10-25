#include "VkPipeline.hpp"

#include "Driver/Vulkan/Pipeline/VkShaderCompiler.hpp"
#include "Driver/Vulkan/Renderer/VkRenderer.hpp"
#include "Mango/Core/Core.hpp"

namespace Mango::Driver::Vulkan {
    namespace {
        namespace Utils {
            vk::Format ShaderDataTypeToVkFormat(ShaderDataType type)
            {
                switch (type)
                {
                    case ShaderDataType::Float:
                        return vk::Format::eR32Sfloat;
                    case ShaderDataType::Float2:
                        return vk::Format::eR32G32Sfloat;
                    case ShaderDataType::Float3:
                        return vk::Format::eR32G32B32Sfloat;
                    case ShaderDataType::Float4:
                        return vk::Format::eR32G32B32A32Sfloat;
                }

                ASSERT(false, "Invalid shader data type");
            }

            std::vector<vk::VertexInputAttributeDescription> VertexInputAttributeDesc(const std::vector<ShaderDataType>& elements)
            {
                std::vector<vk::VertexInputAttributeDescription> result;
                U32 currentStride = 0;
                for (U32 i = 0; i < elements.size(); i++)
                {
                    vk::VertexInputAttributeDescription desc;
                    desc.format = ShaderDataTypeToVkFormat(elements[i]);
                    desc.binding = 0; // TODO: Multiple vertex buffers
                    desc.location = i;
                    desc.offset = currentStride;
                    currentStride += ShaderDataTypeSize(elements[i]);

                    result.emplace_back(desc);
                }

                return result;
            }

            vk::VertexInputBindingDescription VertexInputBindingDesc(const VertexBufferLayout& layout)
            {
                vk::VertexInputBindingDescription desc;
                desc.binding = 0u;
                desc.stride = static_cast<U32>(layout.Stride);
                desc.inputRate = vk::VertexInputRate::eVertex;

                return desc;
            }

            vk::PrimitiveTopology PipelineInputTypeToTopology(PipelineInputType type)
            {
                switch (type)
                {
                    case PipelineInputType::TriangleList:
                        return vk::PrimitiveTopology::eTriangleList;
                    case PipelineInputType::TriangleStrip:
                        return vk::PrimitiveTopology::eTriangleFan;
                    case PipelineInputType::Point:
                        return vk::PrimitiveTopology::ePointList;
                    case PipelineInputType::Line:
                        return vk::PrimitiveTopology::eLineList;
                }

                ASSERT(false, "Invalid pipeline input type");
            }

            vk::PolygonMode PipelinePolygonModeToVk(PipelinePolygonMode mode)
            {
                switch (mode)
                {
                    case PipelinePolygonMode::Fill:
                        return vk::PolygonMode::eFill;
                    case PipelinePolygonMode::Line:
                        return vk::PolygonMode::eLine;
                    case PipelinePolygonMode::Point:
                        return vk::PolygonMode::ePoint;
                }

                ASSERT(false, "Invalid pipeline polygon mode");
            }

            vk::CullModeFlags PipelineCullModeToVk(PipelineCullMode mode)
            {
                switch (mode)
                {
                    case PipelineCullMode::None:
                        return vk::CullModeFlagBits::eNone;
                    case PipelineCullMode::Back:
                        return vk::CullModeFlagBits::eBack;
                    case PipelineCullMode::Front:
                        return vk::CullModeFlagBits::eFront;
                }

                ASSERT(false, "Invalid pipeline cull mode");
            }

            vk::FrontFace PipelineFaceToVk(PipelineFace face)
            {
                switch (face)
                {
                    case PipelineFace::CounterClockWise:
                        return vk::FrontFace::eCounterClockwise;
                    case PipelineFace::ClockWise:
                        return vk::FrontFace::eClockwise;
                }

                ASSERT(false, "Invalid pipeline face");
            }
        } // namespace Utils
    } // namespace

    VkPipeline::VkPipeline(VkRenderer* renderer, const PipelineInfo& info, const std::string& vertShader,
                           const std::string& fragShader) : m_Renderer(renderer)
    {
        vk::SurfaceFormatKHR surfaceFormat = m_Renderer->GetSurfaceFormat();

        vk::PipelineRenderingCreateInfo renderingInfo;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachmentFormats = &surfaceFormat.format;

        vk::GraphicsPipelineCreateInfo pipelineInfo;
        pipelineInfo.pNext = &renderingInfo;

        VkShaderCompiler* shaderCompiler = renderer->GetShaderCompiler();
        VkShaderModulePair modulePair = shaderCompiler->CreateShaderModules(vertShader, fragShader);

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
        vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertShaderStageInfo.module = modulePair.vertShaderModule;
        vertShaderStageInfo.pName = "main";

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
        fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragShaderStageInfo.module = modulePair.fragShaderModule;
        fragShaderStageInfo.pName = "main";

        std::array shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();

        std::vector<vk::VertexInputAttributeDescription> attributes =
                Utils::VertexInputAttributeDesc(info.VertexBufferLayout.Elements);
        vk::VertexInputBindingDescription binding = Utils::VertexInputBindingDesc(info.VertexBufferLayout);

        vk::PipelineVertexInputStateCreateInfo vertexInput;
        vertexInput.vertexBindingDescriptionCount = 1;
        vertexInput.pVertexBindingDescriptions = &binding;
        vertexInput.vertexAttributeDescriptionCount = static_cast<U32>(attributes.size());
        vertexInput.pVertexAttributeDescriptions = attributes.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
        inputAssembly.topology = Utils::PipelineInputTypeToTopology(info.InputType);

        vk::PipelineViewportStateCreateInfo viewportState;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizationState;
        rasterizationState.depthClampEnable = vk::False;
        rasterizationState.rasterizerDiscardEnable = vk::False;
        rasterizationState.polygonMode = Utils::PipelinePolygonModeToVk(info.PolygonMode);
        rasterizationState.cullMode = Utils::PipelineCullModeToVk(info.CullMode);
        rasterizationState.frontFace = Utils::PipelineFaceToVk(info.FrontFace);
        rasterizationState.depthBiasEnable = vk::False;
        rasterizationState.lineWidth = 1.0f;

        vk::PipelineMultisampleStateCreateInfo multiSampleState;
        multiSampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multiSampleState.sampleShadingEnable = vk::False;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.blendEnable = vk::False;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

        vk::PipelineColorBlendStateCreateInfo colorBlendState;
        colorBlendState.logicOpEnable = vk::False;
        colorBlendState.logicOp = vk::LogicOp::eCopy;
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &colorBlendAttachment;

        std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicState;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        pipelineInfo.pVertexInputState = &vertexInput;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizationState;
        pipelineInfo.pMultisampleState = &multiSampleState;
        pipelineInfo.pColorBlendState = &colorBlendState;
        pipelineInfo.pDynamicState = &dynamicState;

        vk::Device d = m_Renderer->GetDevice();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        
        vk::ResultValue<vk::PipelineLayout> layoutRes = d.createPipelineLayout(pipelineLayoutInfo);
        ASSERT(layoutRes.result == vk::Result::eSuccess, "Failed to create pipeline layout");
        m_Layout = layoutRes.value;

        pipelineInfo.layout = m_Layout;

        vk::ResultValue<vk::Pipeline> pipelineRes = d.createGraphicsPipeline(shaderCompiler->GetPipelineCache(), pipelineInfo);
        ASSERT(pipelineRes.result == vk::Result::eSuccess, "Failed to create graphics pipeline");
        m_Pipeline = pipelineRes.value;

        d.destroyShaderModule(modulePair.vertShaderModule);
        d.destroyShaderModule(modulePair.fragShaderModule);
    }

    VkPipeline::~VkPipeline()
    {
        vk::Device d = m_Renderer->GetDevice();

        d.destroyPipeline(m_Pipeline);
        d.destroyPipelineLayout(m_Layout);
    }
} // namespace Mango::Driver::Vulkan
