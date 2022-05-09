#include "Core/Mesh.hpp"

#include <assimp/postprocess.h>
#include <glog/logging.h>

#include <assimp/Importer.hpp>
#include <iostream>

#include "Common.hpp"
#include "Core/Application.hpp"
#include "Core/Component.hpp"
#include "Core/Entity.hpp"
#include "Core/Scene.hpp"
#include "RHI/RHI.hpp"
#include "Tool/EncodingConvert.hpp"

namespace Marbas {

Vector<ElementLayout> GetMeshVertexInfoLayout() {
  return {
      ElementLayout{0, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{1, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{2, ElementType::FLOAT, sizeof(float), 2, false, 0, 0},
  };
};

static Texture2DResource* LoadTexture2D(const aiMaterial* material, aiTextureType type,
                                        const Path& relatePath, ResourceManager* resourceManager) {
  if (material->GetTextureCount(type) == 0) return nullptr;

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
  for (unsigned int i = 0; i < aMesh->mNumVertices; i++) {
    auto vertex = aMesh->mVertices[i];
    auto normal = aMesh->mNormals[i];

    MeshVertexInfo info{
        .posX = vertex.x,
        .posY = vertex.y,
        .posZ = vertex.z,
        .normalX = normal.x,
        .normalY = normal.y,
        .normalZ = normal.z,
    };

    if (texture == nullptr) {
      info.textureU = 0;
      info.textureV = 0;
    } else {
      info.textureU = texture[i].x;
      info.textureV = texture[i].y;
    }

    meshComponent.m_vertices.push_back(info);
  }

  // set face (index buffer)
  for (unsigned int i = 0; i < aMesh->mNumFaces; i++) {
    auto face = aMesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      meshComponent.m_indices.push_back(face.mIndices[j]);
    }
  }
}

void MeshPolicy::ReadMaterialFromNode(const aiMesh* aMesh, const aiScene* aScene, const Path& path,
                                      RenderComponent& renderComponent,
                                      ResourceManager* resourceManager) {
  if (aMesh->mMaterialIndex < 0) return;

  auto* material = aScene->mMaterials[aMesh->mMaterialIndex];

  if (!renderComponent.m_materialResource.has_value()) {
    renderComponent.m_materialResource = resourceManager->AddMaterial()->GetUid();
  }

  Uid uid = renderComponent.m_materialResource.value();
  auto* materialResource = resourceManager->FindMaterialResource(uid);
  if (materialResource == nullptr) return;

  // diffuse texture
  auto diffuseTexture = LoadTexture2D(material, aiTextureType_DIFFUSE, path, resourceManager);

  // ambient Textures
  auto ambientTexture = LoadTexture2D(material, aiTextureType_AMBIENT, path, resourceManager);

  if (diffuseTexture != nullptr) {
    materialResource->AddDiffuseTexture(diffuseTexture->GetUid());
  }

  if (ambientTexture != nullptr) {
    materialResource->AddAmbientTexture(ambientTexture->GetUid());
  }
}

void MeshPolicy::LoadMeshToGPU(Mesh mesh, Scene* scene, RHIFactory* rhiFactory,
                               ResourceManager* resourceManager) {
  if (!Entity::HasComponent<RenderComponent>(scene, mesh)) return;
  auto& renderComponent = Entity::GetComponent<RenderComponent>(scene, mesh);

  if (renderComponent.m_drawBatch == nullptr) {
    renderComponent.m_drawBatch = rhiFactory->CreateDrawBatch();
  }

  // set vertex
  auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, mesh);
  auto vertexCount = meshComponent.m_vertices.size() * sizeof(MeshVertexInfo);
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
  if (!renderComponent.m_materialResource.has_value()) {
    LOG(INFO) << "this mesh don't have a material";
    return;
  }
  auto materialUid = renderComponent.m_materialResource.value();

  auto* materialResource = resourceManager->FindMaterialResource(materialUid);

  auto* material = materialResource->LoadMaterial(resourceManager);
  renderComponent.m_drawBatch->SetMaterial(material);
}

}  // namespace Marbas
