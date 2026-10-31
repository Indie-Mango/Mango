#pragma once
#include "Mango/Common/CommonTypes.hpp"
#include "Mango/Core/Core.hpp"
#include "Mango/Core/Filesystem.hpp"
#include "Mango/Memory/Smart.hpp"
#include "Mango/Renderer/Renderer.hpp"
#include "VertexBuffer.hpp"

namespace Mango {
    enum class PipelineInputType : U8
    {
        TriangleList,
        TriangleStrip,
        Point,
        Line,
    };

    enum class PipelinePolygonMode : U8
    {
        Fill,
        Line,
        Point
    };

    enum class PipelineCullMode : U8
    {
        None,
        Back,
        Front
    };

    enum class PipelineFace : U8
    {
        CounterClockWise,
        ClockWise
    };

    enum class PipelineDescriptorType : U8
    {
        UniformBuffer,
        SampledImage,
    };

    enum class PipelineStage : U8
    {
        None = 0,
        Vertex = BIT(0),
        Fragment = BIT(1),
        All = Vertex | Fragment
    };

    struct PipelineDescriptorSet
    {
        std::vector<PipelineDescriptorType> Descriptors;
        PipelineStage Stage = PipelineStage::None;
    };

    struct PipelineLayout
    {
        std::vector<PipelineDescriptorSet> DescriptorSets;
    };

    struct PipelineInfo
    {
        VertexBufferLayout VertexBufferLayout = {};

        PipelineLayout PipelineLayout;

        PipelineInputType InputType = PipelineInputType::TriangleList;
        PipelinePolygonMode PolygonMode = PipelinePolygonMode::Fill;
        PipelineCullMode CullMode = PipelineCullMode::None;
        PipelineFace FrontFace = PipelineFace::CounterClockWise;
    };

    class Pipeline
    {
    public:
        static Shared<Pipeline> Create(Renderer* renderer, const PipelineInfo& pipelineInfo, const File& vertShader,
                                       const File& fragShader);
        virtual ~Pipeline() = default;

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        Pipeline(Pipeline&&) = delete;
        Pipeline& operator=(Pipeline&&) = delete;

    protected:
        Pipeline() = default;
    };
} // namespace Mango
