#include "Core/Scene/Entity/ModelEntity.hpp"

#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/ModelComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Entity/MeshEntity.hpp"

namespace Marbas {

ModelEntity
ModelEntityPolicy::Create(entt::registry& registry, Uid modelResourceId,
                          const std::shared_ptr<ResourceManager>& resourceManager) {
  // TODO: check is the modelResource is load and existed

  auto modelEntity = registry.create();
  registry.emplace<ModelComponent>(modelEntity);
  registry.emplace<HierarchyComponent>(modelEntity);
  registry.emplace<UniqueTagComponent>(modelEntity);

  auto& modelTagComponent = registry.get<UniqueTagComponent>(modelEntity);
  modelTagComponent.type = EntityType::Model;

  auto& modelComponent = registry.get<ModelComponent>(modelEntity);
  modelComponent.modelResourceId = modelResourceId;

  auto& modelHierarchyComponent = registry.get<HierarchyComponent>(modelEntity);
  modelHierarchyComponent.children.clear();

  auto modelResourceContainer = resourceManager->GetModelResourceContainer();
  auto modelResource = modelResourceContainer->GetResource(modelResourceId);

  // add mesh entity to the registry
  auto model = modelResource->GetModel();
  modelTagComponent.tagName = model->GetModelName();

  auto meshCount = model->GetMeshesCount();
  for (int i = 0; i < meshCount; i++) {
    auto mesh = model->GetMesh(i);
    auto meshEntity = MeshPolicy::Create(mesh, registry);

    auto& meshComponent = registry.get<MeshComponent>(meshEntity);
    meshComponent.m_model = model;

    auto& meshTagComponent = registry.get<UniqueTagComponent>(meshEntity);
    meshTagComponent.tagName = mesh->m_name;
    meshTagComponent.type = EntityType::Mesh;

    auto& hierarchyComponent = registry.get<HierarchyComponent>(meshEntity);
    hierarchyComponent.parent = modelEntity;

    if (!modelHierarchyComponent.children.empty()) {
      auto children = modelHierarchyComponent.children;
      auto lastChild = children[children.size() - 1];
      auto& lastChildHierarchyComponent = registry.get<HierarchyComponent>(lastChild);
      lastChildHierarchyComponent.next = meshEntity;
      hierarchyComponent.prew = lastChild;
    }
    modelHierarchyComponent.children.push_back(meshEntity);
  }

  return modelEntity;
}

}  // namespace Marbas
