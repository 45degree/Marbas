#include "SceneNodeComponent.hpp"

#include "Core/Scene/Component/AABBComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"
#include "Core/Scene/Component/TransformComp.hpp"

namespace Marbas {

void
EmptySceneNode::RegistryNode(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
}

void
DirectionalLightSceneNode::RegistryNode(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
  world.emplace<DirectionLightComponent>(node);
}

void
PointLightSceneNode::RegistryNode(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
}

void
ModelSceneNode::RegistryNode(entt::registry& world, entt::entity node) {
  world.emplace<TransformComp>(node);
}

// void
// LightSceneNode::SetLight(LightType lightType, entt::registry& world) {
//   if (light != entt::null) {
//     world.destroy(light);
//   }
//   light = world.create();
//
//   switch (lightType) {
//     case LightType::ParalleLight:
//       world.emplace<DirectionLightComponent>(light);
//       world.emplace<DirectionShadowComponent>(light);
//       nodeName = "Paralle Light";
//       break;
//     case LightType::PointLight:
//       world.emplace<PointLightComponent>(light);
//       nodeName = "Point Light";
//       break;
//   }
//   // world.emplace<Shad>
// }

// void
// ModelSceneNode::SetModel(entt::registry& world, Path modelPath, RHIFactory* rhiFactory,
//                          ResourceManager* resourceManager) {
//   auto modelResContainer = resourceManager->GetModelResourceContainer();
//   auto modelRes = modelResContainer->CreateResource(modelPath);
//   modelUid = modelResContainer->AddResource(modelRes);
//   modelRes->LoadResource(resourceManager);
//   auto model = modelRes->GetModel();
//   int meshCount = model->GetMeshesCount();
//
//   modelName = model->GetModelName();
//
//   // clear old meshes
//   for (auto oldMesh : meshes) {
//     if (world.any_of<RenderMeshComponent>(oldMesh)) {
//       world.remove<RenderMeshComponent>(oldMesh);
//     }
//     world.destroy(oldMesh);
//   }
//   meshes.clear();
//
//   Vector<AABB> meshAABB;
//   for (int i = 0; i < meshCount; i++) {
//     auto mesh = model->GetMesh(i);
//     if (mesh->m_vertices.empty() || mesh->m_indices.empty()) {
//       continue;
//     }
//     auto meshEntity = world.create();
//     world.emplace<RenderMeshComponent>(meshEntity, mesh, rhiFactory, resourceManager);
//     meshes.push_back(meshEntity);
//
//     meshAABB.push_back(AABB::GenerateAABB(*mesh));
//   }
//
//   aabb = AABB::CombineAABB(meshAABB);
// }

// void
// LightSceneNode::Update(entt::registry& world) {
//   // auto view = world.view<LightSceneNode>();
//   // for (auto entity : view) {
//   //   glm::mat4 globalTranform = glm::mat4(1);
//   //   if (world.any_of<HierarchyComponent>(entity)) {
//   //     globalTranform = world.get<HierarchyComponent>(entity).globalTransformMatrix;
//   //   }
//   //   auto newPos = glm::vec3(glm::column(globalTranform, 3));
//   //   auto& node = world.get<LightSceneNode>(entity);
//   //   // update mesh's render data
//   //   if (node.light != entt::null) {
//   //     if (world.any_of<ParallelLight>(node.light)) {
//   //       auto& paralleLight = world.get<DirectionLightComponent>(node.light).m_light;
//   //       paralleLight.SetPos(newPos);
//   //     } else if (world.any_of<PointLightComponent>(node.light)) {
//   //       auto& pointLight = world.get<PointLightComponent>(node.light).m_light;
//   //       pointLight.SetPos(newPos);
//   //     }
//   //   }
//   // }
// }

// void
// ModelSceneNode::Update(entt::registry& world) {
//   // auto view = world.view<ModelSceneNode>();
//   // for (auto entity : view) {
//   //   glm::mat4 globalTranform = glm::mat4(1);
//   //   if (world.any_of<HierarchyComponent>(entity)) {
//   //     globalTranform = world.get<HierarchyComponent>(entity).globalTransformMatrix;
//   //   }
//   //   auto& node = world.get<ModelSceneNode>(entity);
//   //   // update mesh's render data
//   //   for (auto mesh : node.meshes) {
//   //     if (!world.any_of<RenderMeshComponent>(mesh)) {
//   //       continue;
//   //     }
//   //     auto& renderMeshComponent = world.get<RenderMeshComponent>(mesh);
//   //     renderMeshComponent.meshData.model = globalTranform;
//   //   }
//   // }
// }

}  // namespace Marbas
