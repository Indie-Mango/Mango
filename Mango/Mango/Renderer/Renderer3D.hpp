#pragma once
#include "Renderer.hpp"

namespace Mango {
    struct Renderable3D
    {
        ResourceHandle Resource = 0;
        U32 SubmeshIdx = 0;
        U32 MaterialIdx = 0;
    };

    class Renderer3D
    {
    public:
        Renderer3D() = delete;
        ~Renderer3D() = delete;

        Renderer3D(const Renderer3D&) = delete;
        Renderer3D& operator=(const Renderer3D&) = delete;

        Renderer3D(Renderer3D&&) = delete;
        Renderer3D& operator=(Renderer3D&&) = delete;

        static void Init(Renderer* renderer, ResourceManager* resourceManager);
        static void Shutdown();

        static void DrawList(std::span<Renderable3D> list);
    };
} // namespace Mango
