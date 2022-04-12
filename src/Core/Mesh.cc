#include "Core/Mesh.hpp"
#include "Core/Application.hpp"
#include "RHI/RHI.hpp"
#include "Common.hpp"
#include "Tool/EncodingConvert.hpp"

#include <glog/logging.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

namespace Marbas {

Vector<ElementLayout> GetMeshVertexInfoLayout() {
    return {
        ElementLayout{0, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
        ElementLayout{1, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
        ElementLayout{2, ElementType::FLOAT, sizeof(float), 2, false, 0, 0},
        ElementLayout{3, ElementType::FLOAT, sizeof(int),   2, false, 0, 0},
    };
};

void Mesh::ReadFromNode(const aiMesh* mesh, const aiScene* scene) {

    // set vertex buffer
    auto* texture = mesh->mTextureCoords[0];
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        auto vertex = mesh->mVertices[i];
        auto normal = mesh->mNormals[i];

        MeshVertexInfo info;
        info.posX = vertex.x;
        info.posY = vertex.y;
        info.posZ = vertex.z;
        info.normalX = normal.x;
        info.normalY = normal.y;
        info.normalZ = normal.z;

        if(texture == nullptr) {
            info.textureU = 0;
            info.textureV = 0;
        }
        else {
            info.textureU = texture[i].x;
            info.textureV = texture[i].y;
        }
        m_vertices.push_back(info);
    }

    // set face (index buffer)
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            m_indices.push_back(face.mIndices[j]);
        }
    }

    // load texture path
    if (mesh->mMaterialIndex >= 0) {
        auto* material = scene->mMaterials[mesh->mMaterialIndex];

        // diffuse texture
        m_diffuseTextures =  LoadMaterialTexture(material, aiTextureType_DIFFUSE);

        // ambient Textures
        m_ambientTextures = LoadMaterialTexture(material, aiTextureType_AMBIENT);
    }
}

void Mesh::AddTexturesToMaterial(Material* material) const {
    material->AddDiffuseTextures(m_diffuseTextures);
    material->AddAmbientTextures(m_ambientTextures);
}

Texture2D* Mesh::LoadMaterialTexture(const aiMaterial* material, aiTextureType type) {
    if(material->GetTextureCount(type) == 0) return nullptr;

    aiString str;
    material->GetTexture(type, 0, &str);
    auto texturePath = (m_meshPath / str.C_Str()).string();
#ifdef _WIN32
    std::replace(texturePath.begin(), texturePath.end(), '/', '\\');
#elif __linux__
    std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
#endif
    auto texture = Application::GetRendererFactory()->CreateTexutre2D(texturePath);
    return texture;
}

}  // namespace Marbas
