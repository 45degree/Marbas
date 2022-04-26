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

static Texture2DResource* LoadTexture2D(const aiMaterial* material, aiTextureType type,
                                        const Path& relatePath, ResourceManager* resourceManager) {

    if(material->GetTextureCount(type) == 0) return nullptr;

    aiString str;
    material->GetTexture(type, 0, &str);
    auto texturePath = (relatePath / str.C_Str()).string();
#ifdef _WIN32
    std::replace(texturePath.begin(), texturePath.end(), '/', '\\');
#elif __linux__
    std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
#endif

    auto textureResource = resourceManager->AddTexture(texturePath);
    return textureResource;
}

void MeshPolicy::ReadVertexFromNode(const aiMesh* aMesh, const aiScene* aScene,
                                    MeshComponent& meshComponent) {

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
}

void MeshPolicy::ReadMaterialFromNode(const aiMesh* aMesh, const aiScene* aScene, const Path& path,
                                      RenderComponent& renderComponent,
                                      ResourceManager* resourceManager) {

    if (aMesh->mMaterialIndex < 0) return;

    auto* material = aScene->mMaterials[aMesh->mMaterialIndex];

    // diffuse texture
    renderComponent.m_material = resourceManager->AddMaterial();

    auto diffuseTexture = LoadTexture2D(material, aiTextureType_DIFFUSE, path, resourceManager);

    // ambient Textures
    auto ambientTexture = LoadTexture2D(material, aiTextureType_AMBIENT, path, resourceManager);

    if(renderComponent.m_material == nullptr) {
        renderComponent.m_material = resourceManager->AddMaterial();
    }

    renderComponent.m_material->SetDiffuseTexture(diffuseTexture);
    renderComponent.m_material->SetAmbientTexture(ambientTexture);
}

void MeshPolicy::LoadMeshToGPU(Mesh mesh, Scene* scene, RHIFactory* rhiFactory) {

    if(!Entity::HasComponent<RenderComponent>(scene, mesh)) return;
    auto& renderComponent = Entity::GetComponent<RenderComponent>(scene, mesh);

    if(renderComponent.m_drawBatch == nullptr) {
        renderComponent.m_drawBatch = rhiFactory->CreateDrawBatch();
    }

    // set vertex
    auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, mesh);
    auto vertexCount = meshComponent.m_vertices.size();
    auto vertexBuffer = rhiFactory->CreateVertexBuffer(vertexCount);
    vertexBuffer->SetData(meshComponent.m_vertices.data(), vertexCount, 0);
    vertexBuffer->SetLayout(GetMeshVertexInfoLayout());

    auto indexCount = meshComponent.m_indices.size();
    auto indexBuffer = rhiFactory->CreateIndexBuffer(indexCount);
    indexBuffer->SetData(meshComponent.m_indices, 0);

    auto vertexArray = rhiFactory->CreateVertexArray();

    renderComponent.m_drawBatch->SetVertexBuffer(std::move(vertexBuffer));
    renderComponent.m_drawBatch->SetIndexBuffer(std::move(indexBuffer));
    renderComponent.m_drawBatch->SetVertexArray(std::move(vertexArray));

    // set material
    if(renderComponent.m_material == nullptr) {
        LOG(INFO) << "this mesh don't have a material";
        return;
    }

    auto* material = renderComponent.m_material->GetMaterial();
    renderComponent.m_drawBatch->SetMaterial(material);
}

}  // namespace Marbas
