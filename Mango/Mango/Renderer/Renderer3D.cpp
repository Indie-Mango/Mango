#include "Renderer3D.hpp"

#include "Mango/Graphics/IndexBuffer.hpp"

namespace Mango {
    namespace {
        Renderer* s_Renderer = nullptr;
        MaterialManager* s_MaterialManager =nullptr;
        ResourceManager* s_ResourceManager = nullptr;
    } // namespace

    void Renderer3D::Init(Renderer* renderer, ResourceManager* resourceManager)
    {
        s_Renderer = renderer;
        s_ResourceManager = resourceManager;
    }

    void Renderer3D::Shutdown()
    {
        s_Renderer = nullptr;
        s_ResourceManager = nullptr;
    }

    void Renderer3D::DrawList(std::span<Renderable3D> list)
    {
        for (auto& r : list)
        {
            if (!s_ResourceManager->IsValid(r.Resource) || !s_ResourceManager->IsResourceLoaded(r.Resource))
                continue;
            
            auto* mesh = reinterpret_cast<Mesh*>(s_ResourceManager->ToRaw(r.Resource));
            
            if (mesh->SubMeshes.size() <= r.SubmeshIdx)
                continue;
            
            // TODO: Bind right material
            
            s_Renderer->SetVertexBuffer(mesh->SubMeshes[r.SubmeshIdx].VertexBuffer);
            s_Renderer->SetIndexBuffer(mesh->SubMeshes[r.SubmeshIdx].IndexBuffer);
            
            s_Renderer->DrawIndexed(mesh->SubMeshes[r.SubmeshIdx].IndexBuffer->GetIndexCount());
        }
    }
} // namespace Mango
