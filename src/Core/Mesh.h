#ifndef MARBARS_CORE_MESH_H
#define MARBARS_CORE_MESH_H

#include "Common.h"
#include "RHI/RHI.h"

#include <assimp/scene.h>

namespace Marbas {

class Mesh {
public:
    explicit Mesh(const Path& meshPath) : m_meshPath(meshPath) {};
    ~Mesh() = default;

public:
    void ReadFromNode(const aiMesh* mesh, const aiScene* scene);

    [[nodiscard]] const Vector<float>& GetVertices() const {
        return m_vertices;
    }

    [[nodiscard]] const Vector<uint32_t> GetIndices() const {
        return m_indices;
    }

    [[nodiscard]] Vector<const Texture2D*> GetTexture() const {
        Vector<const Texture2D*> result;
        for(auto texture : m_textures) {
            result.push_back(texture);
        }
        return result;
    }

    void SetMeshName(const String& name) {
        m_meshName = name;
    }

    [[nodiscard]] String GetMeshName() const {
        return m_meshName;
    }

    void LoadToGPU(bool force = false);

    void UnLoadFromGPU();

    DrawUnit* GetDrawUnit() {
        return &m_drawUnit;
    }

private:
    void LoadMaterialTexturePath(const aiMaterial* material, aiTextureType type);

private:
    String m_meshName;
    Path m_meshPath;

    Vector<float> m_vertices;
    Vector<uint32_t> m_indices;
    Vector<Path> m_texturePathes;

    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indicesBuffer;
    Vector<Texture2D*> m_textures;

    DrawUnit m_drawUnit;

    bool m_isLoadToGPU = false;
};

}  // namespace Marbas

#endif
