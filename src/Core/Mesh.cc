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

void MeshPolicy::LoadToGPU(Mesh mesh, Scene* scene, RHIFactory* rhiFactory,
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

CubeMap CubeMapPolicy::Create(entt::registry& registry) {
  auto cubeMap = registry.create();
  registry.emplace<MeshComponent>(cubeMap);
  registry.emplace<CubeMapComponent>(cubeMap);
  registry.emplace<TagsCompoment>(cubeMap);

  auto& tagsCompoment = registry.get<TagsCompoment>(cubeMap);
  tagsCompoment.name = "CubeMap";
  tagsCompoment.type = EntityType::CubeMap;

  auto& meshComponent = registry.get<MeshComponent>(cubeMap);
  meshComponent.m_vertices = {
      MeshVertexInfo{.posX = -1, .posY = -1, .posZ = 1},
      MeshVertexInfo{.posX = 1, .posY = -1, .posZ = 1},
      MeshVertexInfo{.posX = 1, .posY = 1, .posZ = 1},
      MeshVertexInfo{.posX = -1, .posY = 1, .posZ = 1},
      MeshVertexInfo{.posX = -1, .posY = -1, .posZ = -1},
      MeshVertexInfo{.posX = 1, .posY = -1, .posZ = -1},
      MeshVertexInfo{.posX = 1, .posY = 1, .posZ = -1},
      MeshVertexInfo{.posX = -1, .posY = 1, .posZ = -1},
  };
  meshComponent.m_indices = {3, 0, 1, 3, 1, 2, 2, 1, 6, 6, 1, 5, 6, 3, 2, 7, 3, 6,
                             1, 0, 4, 1, 4, 5, 7, 0, 3, 0, 7, 4, 4, 7, 5, 7, 6, 5};

  return cubeMap;
}

void CubeMapPolicy::LoadToGPU(CubeMap cubeMap, Scene* scene, RHIFactory* rhiFactory,
                              ResourceManager* resourceManager) {
  if (!Entity::HasComponent<CubeMapComponent>(scene, cubeMap)) return;
  auto& cubeMapComponent = Entity::GetComponent<CubeMapComponent>(scene, cubeMap);

  if (cubeMapComponent.m_drawBatch == nullptr) {
    cubeMapComponent.m_drawBatch = rhiFactory->CreateDrawBatch();
  }

  // set vertex
  auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, cubeMap);
  auto vertexCount = meshComponent.m_vertices.size() * sizeof(MeshVertexInfo);
  auto vertexBuffer = rhiFactory->CreateVertexBuffer(vertexCount);
  vertexBuffer->SetData(meshComponent.m_vertices.data(), vertexCount, 0);
  vertexBuffer->SetLayout(GetMeshVertexInfoLayout());

  auto indexCount = meshComponent.m_indices.size();
  auto indexBuffer = rhiFactory->CreateIndexBuffer(indexCount);
  indexBuffer->SetData(meshComponent.m_indices, 0);

  auto vertexArray = rhiFactory->CreateVertexArray();

  cubeMapComponent.m_drawBatch->SetVertexBuffer(std::move(vertexBuffer));
  cubeMapComponent.m_drawBatch->SetIndexBuffer(std::move(indexBuffer));
  cubeMapComponent.m_drawBatch->SetVertexArray(std::move(vertexArray));

  // set material
  if (!cubeMapComponent.m_cubeMapResource.has_value()) {
    LOG(INFO) << "this mesh don't have a material";
    return;
  }
  auto materialUid = cubeMapComponent.m_cubeMapResource.value();

  auto* materialResource = resourceManager->FindCubeMapMaterialResource(materialUid);

  auto* material = materialResource->LoadResource(resourceManager);
  cubeMapComponent.m_drawBatch->SetMaterial(material);
}

void CubeMapPolicy::ReadCubeMapFromFile(const CubeMapCreateInfo& createInfo,
                                        CubeMapComponent& component,
                                        ResourceManager* resourceManager) {
  auto* cubeMapTextureResource = resourceManager->AddCubeMapMaterial(createInfo);
  component.m_cubeMapResource = cubeMapTextureResource->GetUid();
}

}  // namespace Marbas
