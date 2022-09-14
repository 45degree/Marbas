#include "Core/Scene/Entity/MeshEntity.hpp"

#include <assimp/postprocess.h>
#include <glog/logging.h>

#include <assimp/Importer.hpp>
#include <iostream>

#include "Common/Common.hpp"
#include "Core/Application.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHI/RHI.hpp"
#include "Tool/EncodingConvert.hpp"

namespace Marbas {

static std::shared_ptr<Texture2DResource>
LoadTexture2D(const aiMaterial* material, aiTextureType type, const Path& relatePath,
              ResourceManager* resourceManager) {
  if (material->GetTextureCount(type) == 0) return nullptr;

  aiString str;
  material->GetTexture(type, 0, &str);
  auto texturePath = (relatePath / str.C_Str()).string();
#ifdef _WIN32
  std::replace(texturePath.begin(), texturePath.end(), '/', '\\');
#elif __linux__
  std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
#endif

  // TODO: need to check if the resource is existed
  auto container = resourceManager->GetTexture2DResourceContainer();
  auto resource = container->CreateResource(Path(texturePath));
  container->AddResource(resource);
  return resource;
}

MeshEntity
MeshPolicy::Create(const std::shared_ptr<Mesh>& mesh, entt::registry& registry) {
  auto meshEntity = registry.create();
  registry.emplace<MeshComponent>(meshEntity);
  registry.emplace<HierarchyComponent>(meshEntity);
  registry.emplace<UniqueTagComponent>(meshEntity);
  registry.emplace<ShadowComponent>(meshEntity);

  auto& meshComponent = registry.get<MeshComponent>(meshEntity);
  meshComponent.m_mesh = mesh;

  return meshEntity;
}

}  // namespace Marbas
