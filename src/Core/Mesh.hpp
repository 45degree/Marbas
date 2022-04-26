#ifndef MARBAS_CORE_MESH_H
#define MARBAS_CORE_MESH_H

#include "Common.hpp"
#include "RHI/RHI.hpp"
#include "Core/Component.hpp"
#include "entt/entt.hpp"

#include <assimp/scene.h>

namespace Marbas {

extern Vector<ElementLayout> GetMeshVertexInfoLayout();

using Mesh = entt::entity;

class Scene;
class MeshCreatePolicy {
public:
    MeshCreatePolicy() = delete;
    MeshCreatePolicy(const MeshCreatePolicy&) = delete;
    MeshCreatePolicy& operator=(const MeshCreatePolicy&) = delete;

    static entt::entity Create(entt::registry& registry) {
        auto mesh = registry.create();
        registry.emplace<MeshComponent>(mesh);
        return mesh;
    }

    static Mesh CreateMeshFromNode(const aiMesh* aMesh, const aiScene* aScene, Scene* scene,
                                   const Path& relativePath, ResourceManager* resourceManager);
};


// class Mesh : public Entity {
// public:
//     explicit Mesh(Scene* scene);
//     ~Mesh() = default;
//
//     Mesh(const Mesh&) = delete;
//     Mesh& operator=(const Mesh&) = delete;
//
// public:
//
//     void ReadFromNode(const aiMesh* mesh, const aiScene* scene, const Path& relativePath,
//                       ResourceManager* resourceManager);
//
// private:
//     Texture2DResource* LoadTexture2D(const aiMaterial* material, aiTextureType type, 
//                                      const Path& relativePath, ResourceManager* resourceManager);
// };

// class Mesh {
// public:
//     explicit Mesh(const Path& meshPath) : m_meshPath(meshPath) {};
//     ~Mesh() = default;
//
// public:
//     void ReadFromNode(const aiMesh* mesh, const aiScene* scene);
//
//     [[nodiscard]] const Vector<MeshVertexInfo>& GetVertices() const {
//         return m_vertices;
//     }
//
//     [[nodiscard]] const Vector<uint32_t> GetIndices() const {
//         return m_indices;
//     }
//
//     void SetMeshName(const String& name) {
//         m_meshName = name;
//     }
//
//     [[nodiscard]] size_t GetVertexByte() const {
//         return sizeof(decltype(m_vertices[0])) * m_vertices.size();
//     }
//
//     [[nodiscard]] size_t GetVertexCount() const {
//         return m_vertices.size();
//     }
//
//     [[nodiscard]] size_t GetIndicesCount() const noexcept {
//         return m_indices.size();
//     }
//
//     [[nodiscard]] String GetMeshName() const {
//         return m_meshName;
//     }
//
//     [[nodiscard]] Texture2D* GetDiffuseTexture() const noexcept {
//         return m_diffuseTextures;
//     }
//
//     [[nodiscard]] Texture2D* GetAmbientTexture() const noexcept {
//         return m_ambientTextures;
//     }
//
//     void AddTexturesToMaterial(Material* material) const;
//
// private:
//     Texture2D* LoadMaterialTexture(const aiMaterial* material, aiTextureType type);
//
// private:
//     String m_meshName;
//     Path m_meshPath;
//
//     Vector<MeshVertexInfo> m_vertices;
//     Vector<uint32_t> m_indices;
//
//     Texture2D* m_ambientTextures = nullptr;
//     Texture2D* m_diffuseTextures = nullptr;
// };

}  // namespace Marbas

#endif
