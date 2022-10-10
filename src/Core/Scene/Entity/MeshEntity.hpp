#pragma once

#include <assimp/scene.h>

#include "Common/Common.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "RHI/RHI.hpp"
#include "Resource/ResourceManager.hpp"
#include "entt/entt.hpp"

namespace Marbas {

using MeshEntity = entt::entity;

class Scene;
class MeshPolicy {
 public:
  MeshPolicy() = delete;
  MeshPolicy(const MeshPolicy&) = delete;
  MeshPolicy&
  operator=(const MeshPolicy&) = delete;

  static MeshEntity
  Create(const std::shared_ptr<Mesh>& mesh, entt::registry& registry);

  static void
  ReadVertexFromNode(const aiMesh* aMesh, const aiScene* aScene, MeshComponent& meshComponent);

  static void
  ReadMaterialFromNode(const aiMesh* aMesh, const aiScene* aScene, const Path& path,
                       MeshComponent& meshComponent, ResourceManager* resourceManager);

  static void
  LoadToGPU(MeshEntity mesh, Scene* scene, RHIFactory* rhiFactory,
            ResourceManager* resourceManager);
};

// using CubeMap = entt::entity;
// class CubeMapPolicy {
//  public:
//   static CubeMap
//   Create(entt::registry& registry);
//
//   static void
//   LoadToGPU(CubeMap cubeMap, Scene* scene, RHIFactory* rhiFactory,
//             ResourceManager* resourceManager);
//
//   // static void
//   // ReadCubeMapFromFile(const CubeMapCreateInfo& creteInfo, CubeMapComponent& component,
//   //                     ResourceManager* resourceManager);
// };

}  // namespace Marbas
