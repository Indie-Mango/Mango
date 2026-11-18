#pragma once
#include "Mango/Core/Filesystem.hpp"
#include "Mango/Memory/Smart.hpp"

#include <fastgltf/types.hpp>
#include <glm/glm.hpp>

namespace Mango {
    class Engine;
    class Renderer;
    class VertexBuffer;
    class IndexBuffer;

    struct Resource
    {
        Resource() = default;
        Resource(const Path& path) : File(path, FileMode::Read) {}
        virtual ~Resource() = default;

        Resource(const Resource& other) = delete;
        Resource& operator=(const Resource& other) = delete;

        Resource(Resource&& other) = delete;
        Resource& operator=(Resource&& other) = delete;

        virtual bool Load() = 0;
        virtual void Unload() = 0;

        bool IsLoaded() const { return Loaded; }

        File File;
        bool Loaded = false;
    };

    struct Picture : Resource
    {
        Picture() = default;
        Picture(const Path& path) : Resource(path) {}
        ~Picture() override = default;

        Picture(const Picture& other) = delete;
        Picture& operator=(const Picture& other) = delete;

        Picture(Picture&& other) = delete;
        Picture& operator=(Picture&& other) = delete;

        bool Load() override;
        void Unload() override;

        U32 Width = 0, Height = 0, Channels = 0;
        U8* Data = nullptr;
    };

    struct MeshVertex
    {
        glm::vec3 Position{};
        glm::vec3 Normal{};
        glm::vec2 TexCoord{};
    };

    struct SubMesh
    {
        Shared<VertexBuffer> VertexBuffer;
        Shared<IndexBuffer> IndexBuffer;

        MaterialHandle Material;
    };

    struct MeshMaterial
    {
        glm::vec4 BaseColor{1.0f};
        glm::vec3 EmissiveColor{0.0f};

        float Metallic = 1.0f;
        float Roughness = 1.0f;

        bool DoubleSided = false;
        float AlphaCutoff = 0.5f;

        fastgltf::AlphaMode AlphaMode = fastgltf::AlphaMode::Opaque;
    };

    struct Mesh : Resource
    {
        Mesh() = default;
        Mesh(const Path& path, Engine* engine, Renderer* renderer) : Resource(path), Renderer(renderer), Engine(engine) {}
        ~Mesh() override = default;

        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other) = delete;

        Mesh(Mesh&& other) = delete;
        Mesh& operator=(Mesh&& other) = delete;

        bool Load() override;
        void Unload() override;

        std::vector<SubMesh> SubMeshes;
        std::vector<MaterialHandle> Materials;
        Engine* Engine = nullptr;
        Renderer* Renderer = nullptr;

    private:
        MaterialHandle ImportMaterial(const fastgltf::Material& src, const fastgltf::Asset& asset) const;
    };
} // namespace Mango
