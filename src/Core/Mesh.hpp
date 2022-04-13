#ifndef MARBAS_CORE_MESH_H
#define MARBAS_CORE_MESH_H

#include "Common.hpp"
#include "RHI/RHI.hpp"

#include <assimp/scene.h>

namespace Marbas {

#pragma pack(1)
extern "C" struct MeshVertexInfo {
    float posX = 0.f, posY = 0.f, posZ = 0.f;
    float normalX = 0.f, normalY = 0.f, normalZ = 0.f;
    float textureU = 0.f, textureV = 0.f;
    int32_t diffuseTextureId = 0, ambientTextureId = 0;
};
#pragma pack()

extern Vector<ElementLayout> GetMeshVertexInfoLayout();

class Mesh {
public:
    explicit Mesh(const Path& meshPath) : m_meshPath(meshPath) {};
    ~Mesh() = default;

public:
    void ReadFromNode(const aiMesh* mesh, const aiScene* scene);

    [[nodiscard]] const Vector<MeshVertexInfo>& GetVertices() const {
        return m_vertices;
    }

    [[nodiscard]] const Vector<uint32_t> GetIndices() const {
        return m_indices;
    }

    void SetMeshName(const String& name) {
        m_meshName = name;
    }

    [[nodiscard]] size_t GetVertexByte() const {
        return sizeof(decltype(m_vertices[0])) * m_vertices.size();
    }

    [[nodiscard]] size_t GetVertexCount() const {
        return m_vertices.size();
    }

    [[nodiscard]] size_t GetIndicesCount() const noexcept {
        return m_indices.size();
    }

    [[nodiscard]] String GetMeshName() const {
        return m_meshName;
    }

    [[nodiscard]] Texture2D* GetDiffuseTexture() const noexcept {
        return m_diffuseTextures;
    }

    [[nodiscard]] Texture2D* GetAmbientTexture() const noexcept {
        return m_ambientTextures;
    }

    void AddTexturesToMaterial(Material* material) const;

private:
    Texture2D* LoadMaterialTexture(const aiMaterial* material, aiTextureType type);

private:
    String m_meshName;
    Path m_meshPath;

    Vector<MeshVertexInfo> m_vertices;
    Vector<uint32_t> m_indices;

    Texture2D* m_ambientTextures = nullptr;
    Texture2D* m_diffuseTextures = nullptr;
};

}  // namespace Marbas

#endif
