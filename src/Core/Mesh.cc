#include "Core/Mesh.hpp"
#include "Core/Scene.hpp"
#include "Core/Entity.hpp"
#include "Core/Component.hpp"
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
        ElementLayout{3, ElementType::INT, sizeof(int),     2, false, 0, 0},
    };
};

Mesh MeshCreatePolicy::CreateMeshFromNode(const aiMesh* aMesh, const aiScene* aScene,
                                          Scene* scene, const Path& relativePath,
                                          ResourceManager* resourceManager) {

    auto mesh = Entity::CreateEntity<MeshCreatePolicy>(scene);

    auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, mesh);
    auto& renderComponent = Entity::GetComponent<RenderComponent>(scene, mesh);

    // set vertex buffer
    auto* texture = aMesh->mTextureCoords[0];
    for(unsigned int i = 0; i < aMesh->mNumVertices; i++) {
        auto vertex = aMesh->mVertices[i];
        auto normal = aMesh->mNormals[i];

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
        meshComponent.m_vertices.push_back(info);
    }

    // set face (index buffer)
    for(unsigned int i = 0; i < aMesh->mNumFaces; i++) {
        auto face = aMesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            meshComponent.m_indices.push_back(face.mIndices[j]);
        }
    }

    // load texture path
    if (aMesh->mMaterialIndex >= 0) {
        auto* material = aScene->mMaterials[aMesh->mMaterialIndex];

        // diffuse texture
        renderComponent.m_diffuseTexture = LoadTexture2D(material, aiTextureType_DIFFUSE,
                                                         relativePath, resourceManager);

        // ambient Textures
        renderComponent.m_ambientTexture = LoadTexture2D(material, aiTextureType_AMBIENT,
                                                         relativePath, resourceManager);
    }

    return mesh;
}
//
// Texture2DResource* LoadTexture2D(const aiMaterial* material, aiTextureType type,
//                                  const Path& relatePath, ResourceManager* resourceManager) {
//
//     if(material->GetTextureCount(type) == 0) return nullptr;
//
//     aiString str;
//     material->GetTexture(type, 0, &str);
//     auto texturePath = (relatePath / str.C_Str()).string();
// #ifdef _WIN32
//     std::replace(texturePath.begin(), texturePath.end(), '/', '\\');
// #elif __linux__
//     std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
// #endif
//
//     auto textureResource = resourceManager->AddTexture(texturePath);
//     return textureResource;
// }

}  // namespace Marbas
