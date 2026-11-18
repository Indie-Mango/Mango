#include "Resource.hpp"

#include "Mango/Graphics/IndexBuffer.hpp"
#include "Mango/Graphics/UniformBuffer.hpp"
#include "Mango/Graphics/VertexBuffer.hpp"
#include "Mango/Main/Engine.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <stb/stb_image.h>

namespace Mango {
    bool Picture::Load() // TODO: Don't hardcode desired channels
    {
        Loaded = false;

        if (!File.IsOpen())
            return false;

        int w, h, c;
        Data = stbi_load(File.String().c_str(), &w, &h, &c, STBI_rgb_alpha);

        Width = w;
        Height = h;
        Channels = c;

        Loaded = true;
        return Data;
    }

    void Picture::Unload()
    {
        Width = 0;
        Height = 0;
        Channels = 0;

        stbi_image_free(Data);

        Loaded = false;
    }

    bool Mesh::Load()
    {
        Loaded = false;

        SubMeshes.clear();
        Materials.clear();

        fastgltf::Parser parser;

        auto data = fastgltf::GltfDataBuffer::FromPath(File.GetPath());

        if (data.error() != fastgltf::Error::None)
        {
            Logger::Error("Failed to open glTF file");
            return false;
        }

        constexpr auto options = fastgltf::Options::LoadExternalBuffers;

        auto result = parser.loadGltf(data.get(), File.GetPath().parent_path(), options);

        if (result.error() != fastgltf::Error::None)
        {
            Logger::Error("Failed to parse glTF file");
            return false;
        }

        const auto& asset = result.get();

        Materials.reserve(asset.materials.size());

        for (const auto& gltfMaterial : asset.materials)
        {
            MaterialHandle handle = ImportMaterial(gltfMaterial, asset);

            Materials.push_back(handle);
        }

        for (const auto& mesh : asset.meshes)
        {
            for (const auto& primitive : mesh.primitives)
            {
                if (primitive.type != fastgltf::PrimitiveType::Triangles)
                    continue;

                const auto* positionAttribute = primitive.findAttribute("POSITION");

                if (!positionAttribute)
                    continue;

                const auto& positionAccessor = asset.accessors[positionAttribute->accessorIndex];

                std::vector<MeshVertex> vertices(positionAccessor.count);
                std::vector<U32> indices;

                // Positions
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                        asset, positionAccessor, [&](fastgltf::math::fvec3 value, USize index) {
                            vertices[index].Position = {value.x(), value.y(), value.z()};
                        });

                // Normals
                if (const auto* attribute = primitive.findAttribute("NORMAL"))
                {
                    const auto& accessor = asset.accessors[attribute->accessorIndex];

                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                            asset, accessor, [&](fastgltf::math::fvec3 value, USize index) {
                                vertices[index].Normal = {value.x(), value.y(), value.z()};
                            });
                }

                // TexCoords
                if (const auto* attribute = primitive.findAttribute("TEXCOORD_0"))
                {
                    const auto& accessor = asset.accessors[attribute->accessorIndex];

                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                            asset, accessor,
                            [&](fastgltf::math::fvec2 value, USize index) { vertices[index].TexCoord = {value.x(), value.y()}; });
                }

                // Indices
                if (primitive.indicesAccessor.has_value())
                {
                    const auto& accessor = asset.accessors[*primitive.indicesAccessor];

                    indices.resize(accessor.count);

                    fastgltf::iterateAccessorWithIndex<U32>(asset, accessor, [&](U32 value, USize index) { indices[index] = value; });
                } else
                {
                    indices.reserve(vertices.size());

                    for (USize i = 0; i < vertices.size(); ++i)
                    {
                        indices.push_back(static_cast<U32>(i));
                    }
                }

                SubMesh submesh;
                submesh.VertexBuffer = VertexBuffer::Create(
                        Renderer, VertexBufferLayout{ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2},
                        static_cast<U32>(vertices.size()), vertices.data());
                submesh.IndexBuffer = IndexBuffer::Create(Renderer, indices);
                submesh.Material = Materials[primitive.materialIndex.value()];

                SubMeshes.push_back(std::move(submesh));
            }
        }

        Loaded = true;
        return true;
    }

    void Mesh::Unload()
    {
        SubMeshes.clear();
        Materials.clear();

        Loaded = false;
    }

    MaterialHandle Mesh::ImportMaterial(const fastgltf::Material& src, const fastgltf::Asset& asset) const
    {
        struct alignas(16) PBRMaterialData
        {
            glm::vec4 BaseColorFactor = {1.f, 1.f, 1.f, 1.f};
            glm::vec4 EmissiveFactor = {0.f, 0.f, 0.f, 0.f};

            float MetallicFactor = 1.f;
            float RoughnessFactor = 1.f;
            float AlphaCutoff = 0.5f;
            U32 Flags = 0;
        };

        enum PBRMaterialFlags : U32
        {
            HasBaseColorTexture = 1u << 0,
            HasMetallicRoughnessTexture = 1u << 1,
            AlphaMask = 1u << 2,

            HasNormalTexture = 1u << 3,
            HasOcclusionTexture = 1u << 4,
            HasEmissiveTexture = 1u << 5
        };

        auto& manager = Renderer->GetMaterialManager();

        // 1. Create engine material.
        // Adjust arguments to your Material::Create signature.
        MaterialHandle material = manager.CreateMaterial(nullptr, 0); // TODO

        // 2. Convert glTF properties.
        const auto& pbr = src.pbrData;

        PBRMaterialData data{};

        data.BaseColorFactor = {static_cast<float>(pbr.baseColorFactor[0]), static_cast<float>(pbr.baseColorFactor[1]),
                                static_cast<float>(pbr.baseColorFactor[2]), static_cast<float>(pbr.baseColorFactor[3])};

        data.EmissiveFactor = {static_cast<float>(src.emissiveFactor[0]), static_cast<float>(src.emissiveFactor[1]),
                               static_cast<float>(src.emissiveFactor[2]), 0.0f};

        data.MetallicFactor = static_cast<float>(pbr.metallicFactor);

        data.RoughnessFactor = static_cast<float>(pbr.roughnessFactor);

        data.AlphaCutoff = static_cast<float>(src.alphaCutoff);

        if (src.alphaMode == fastgltf::AlphaMode::Mask)
            data.Flags |= AlphaMask;

        // 3. Import textures and bind material descriptors.
        // These helpers are engine-specific and should be
        // implemented using your existing Texture API.

        auto importTexture = [&](std::size_t textureIndex, U32 binding, U32 flag, bool srgb) {
            const auto& gltfTexture = asset.textures.at(textureIndex);

            // Implement:
            // - Resolve gltfTexture.imageIndex
            // - Load image bytes from asset.images
            // - Decode image
            // - Create Mango::Texture with correct format
            // - Cache by image + sampler + color space

            // manager.SetTexture(material, 1, binding, texture);
            data.Flags |= flag;
        };

        if (pbr.baseColorTexture)
        {
            importTexture(pbr.baseColorTexture->textureIndex, 1, HasBaseColorTexture, true);
        }

        if (pbr.metallicRoughnessTexture)
        {
            importTexture(pbr.metallicRoughnessTexture->textureIndex, 2, HasMetallicRoughnessTexture, false);
        }

        if (src.normalTexture)
        {
            importTexture(src.normalTexture->textureIndex, 3, HasNormalTexture, false);
        }

        if (src.occlusionTexture)
        {
            importTexture(src.occlusionTexture->textureIndex, 4, HasOcclusionTexture, false);
        }

        if (src.emissiveTexture)
        {
            importTexture(src.emissiveTexture->textureIndex, 5, HasEmissiveTexture, true);
        }

        Shared<UniformBuffer> buffer = UniformBuffer::Create(Renderer, sizeof(PBRMaterialData));
        buffer->Write(&data, sizeof(PBRMaterialData));
        manager.SetUniformBuffer(material, 1, 0, buffer);

        return material;
    }
} // namespace Mango

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
