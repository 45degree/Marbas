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

    void LoadToGPU(bool force = false);

    void UnLoadFromGPU();

    void SetShader(Shader* shader);

    DrawUnit* GetDrawUnit() {
        return &m_drawUnit;
    }

private:
    void LoadMaterialTexturePath(const aiMaterial* material, aiTextureType type);

private:
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
