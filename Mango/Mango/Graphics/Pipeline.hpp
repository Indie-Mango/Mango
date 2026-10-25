#pragma once
#include "Mango/Common/CommonTypes.hpp"
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

    struct PipelineInfo
    {
        VertexBufferLayout VertexBufferLayout = {};

        PipelineInputType InputType = PipelineInputType::TriangleList;
        PipelinePolygonMode PolygonMode = PipelinePolygonMode::Fill;
        PipelineCullMode CullMode = PipelineCullMode::None;
        PipelineFace FrontFace = PipelineFace::CounterClockWise;
    };

    class Pipeline
    {
    public:
        static Shared<Pipeline> Create(Renderer* renderer, const PipelineInfo& pipelineInfo, const std::string& vertShader,
                                       const std::string& fragShader);
        virtual ~Pipeline() = default;

        Pipeline(const Pipeline& other) = delete;
        Pipeline& operator=(const Pipeline& other) = delete;
        
        Pipeline(Pipeline&& other) = delete;
        Pipeline& operator=(Pipeline&& other) = delete;

    protected:
        Pipeline() = default;
    };
} // namespace Mango
