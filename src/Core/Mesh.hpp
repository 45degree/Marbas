#ifndef MARBAS_CORE_MESH_H
#define MARBAS_CORE_MESH_H

#include "Common.hpp"
#include "RHI/RHI.hpp"

#include <assimp/scene.h>

namespace Marbas {

#pragma pack(1)
extern "C" struct MeshVertexInfo {
    float posX, posY, posZ;
    float normalX, normalY, normalZ;
    float textureU, textureV;
    int textureId;
};
#pragma pack()

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

    [[nodiscard]] String GetMeshName() const {
        return m_meshName;
    }

    void AddTexturesToMaterial(Material* material) const;

private:
    Vector<Texture2D*> LoadMaterialTexture(const aiMaterial* material, aiTextureType type);

private:
    String m_meshName;
    Path m_meshPath;

    Vector<MeshVertexInfo> m_vertices;
    Vector<uint32_t> m_indices;

    Vector<Texture2D*> m_textures;
    Vector<Texture2D*> m_ambientTextures;
    Vector<Texture2D*> m_diffuseTextures;

    bool m_isLoadToGPU = false;
};

}  // namespace Marbas

#endif
