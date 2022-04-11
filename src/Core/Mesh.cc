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
        auto diffuseTextures = LoadMaterialTexture(material, aiTextureType_DIFFUSE);
        m_diffuseTextures.insert(m_diffuseTextures.end(), diffuseTextures.begin(),
                                 diffuseTextures.end());

        // ambient Textures
        auto ambientTextures = LoadMaterialTexture(material, aiTextureType_AMBIENT);
        m_ambientTextures.insert(m_ambientTextures.end(), ambientTextures.begin(),
                                 ambientTextures.end());

    }
}

void Mesh::AddTexturesToMaterial(Material* material) const {
    for(auto* texture : m_diffuseTextures) {
        material->AddDiffuseTextures(texture);
    }

    for(auto* texture : m_ambientTextures) {
        material->AddAmbientTextures(texture);
    }
}

Vector<Texture2D*> Mesh::LoadMaterialTexture(const aiMaterial* material, aiTextureType type) {
    Vector<Texture2D*> result;
    for(auto i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, i, &str);
        auto texturePath = (m_meshPath / str.C_Str()).string();
#ifdef _WIN32
        std::replace(texturePath.begin(), texturePath.end(), '/', '\\');
#elif __linux__
        std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
#endif
        auto texture = Application::GetRendererFactory()->CreateTexutre2D(texturePath);
        result.push_back(texture);
    }
    return result;
}

}  // namespace Marbas
