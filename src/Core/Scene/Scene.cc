#include "Core/Scene/Scene.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <algorithm>
#include <assimp/Importer.hpp>

#include "Common/Common.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Entity/BillBoardEntity.hpp"
#include "Core/Scene/Entity/CubeMapEntity.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Scene/Entity/EnvironmentEntity.hpp"
#include "Core/Scene/Entity/LightEntity.hpp"
#include "Core/Scene/Entity/MeshEntity.hpp"
#include "Core/Scene/Entity/ModelEntity.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

// TODO: need to impl
static void
ProcessModelNode(const toml::table& modelNode, entt::registry& world,
                 std::shared_ptr<IResourceContainer<ModelResource>>& modelResourceContainer) {
  // // load model
  // const auto path = Path(modelNode["path"].as_string()->get());
  //
  // // TODO: need to change
  // auto modelResource = modelResourceContainer->CreateResource();
  // auto uid = modelResourceContainer->AddResource(modelResource);
  //
  // // auto modelResource = auto modelResource = resourceManager->AddModel(path);
  // auto modelEntity = ModelEntityPolicy::Create(world, modelResource);
  //
  // // set material for every mesh
  // const auto& meshNodes = *modelNode["mesh"].as_array();
  // const auto model = modelResource->GetModel();
  // for (const auto& meshNode : meshNodes) {
  //   meshNode.visit([&](const toml::table& elem) {
  //     auto index = (*elem["meshIndex"].as_integer()).get();
  //     auto materialId = (*elem["materialId"].as_integer()).get();
  //
  //     const auto mesh = model->GetMesh(index);
  //     auto meshEntity = MeshPolicy::Create(mesh, world);
  //     auto meshComponent = world.get<MeshComponent>(meshEntity);
  //     meshComponent.m_materialResourceId = Uid(materialId);
  //   });
  // }
}

// TODO: need to impl
static void
ProcessNode(const toml::array& sceneNodes, entt::registry& world,
            const std::shared_ptr<ResourceManager>& resourceManager) {
  // for (const toml::node& elem : sceneNodes) {
  //   elem.visit([&](const toml::table& node) {
  //     auto type = (*node["type"].as_string()).ref<std::string>();
  //     if (type == "Model") {
  //       // Parse the node as an model
  //       auto modelResourceContainer = resourceManager->GetContainer<ModelResource>();
  //       ProcessModelNode(node, world, modelResourceContainer);
  //
  //       // parse the subnode
  //       const auto* subNodesPtr = node["node"].as_array();
  //       if (subNodesPtr != nullptr && subNodesPtr->size() != 0) {
  //         ProcessNode(*subNodesPtr, world, resourceManager);
  //       }
  //     } else if (type == "Light") {
  //       // TODO: process light
  //     } else if (type == "") {
  //       // TODO: add others
  //     }
  //   });
  // }
}

Scene::Scene(const std::shared_ptr<ResourceManager>& resourceManager)
    : m_resourceManager(resourceManager) {
  m_rootEntity = m_world.create();
  m_world.emplace<HierarchyComponent>(m_rootEntity);
  m_world.emplace<UniqueTagComponent>(m_rootEntity);
  m_editorCamera = std::make_shared<EditorCamera>();

  auto& tagComponent = m_world.get<UniqueTagComponent>(m_rootEntity);
  tagComponent.tagName = m_name;

  /**
   * add default cubemap
   */

  // create default cubeMap texture
  auto cubeMapResourceContainer = m_resourceManager->GetCubeMapResourceContainer();
  auto cubeMapResource = cubeMapResourceContainer->CreateResource(CubeMapCreateInfo{
      .top = "assert/skybox/top.jpg",
      .bottom = "assert/skybox/bottom.jpg",
      .back = "assert/skybox/back.jpg",
      .front = "assert/skybox/front.jpg",
      .left = "assert/skybox/left.jpg",
      .right = "assert/skybox/right.jpg",
  });
  auto cubeMapResourceId = cubeMapResourceContainer->AddResource(cubeMapResource);

  // create Entity
  auto cubeMap = CubeMapPolicy::Create(m_world, cubeMapResourceId);
  HierarchyComponent::AddChild(m_rootEntity, m_world, cubeMap);

  // TODO: need to remove
  AddLight(LightType::PointLight, glm::vec3(0, 30, 0), m_rootEntity);

  Entity::CreateEntity<EnvironmentPolicy>(
      this, "assert/sIBL_Collection/apartment/Apartment_Spherical_HiRes.jpg");
}

void
Scene::ProcessScene(const toml::table& sceneTomlTable) {
  auto name = sceneTomlTable["scene"]["name"].value<std::string>();
  if (name.has_value()) {
    m_name = name.value();
  } else {
    LOG(WARNING) << "the scene don't have a name";
  }

  // load resource

  // load others
  const auto nodes = *sceneTomlTable["scene"]["node"].as_array();
  ProcessNode(nodes, m_world, m_resourceManager);
}

void
Scene::ReadFromFile(const Path& scenePath) {
  toml::table sceneTomlTable;
  try {
    sceneTomlTable = toml::parse_file(scenePath.c_str());
  } catch (const toml::parse_error& err) {
    LOG(ERROR) << FORMAT("can't parse file: {}, maybe is not a valid toml file", scenePath);
    return;
  }

  ProcessScene(sceneTomlTable);
}

void
Scene::ReadFromString(const String& sceneStr) {
  toml::table sceneTomlTable;
  try {
    sceneTomlTable = toml::parse(sceneStr.c_str());
  } catch (const toml::parse_error& err) {
    LOG(ERROR) << FORMAT("can't parse string: {}, maybe is not a valid toml file", sceneStr);
    return;
  }

  ProcessScene(sceneTomlTable);
}

// TODO: need to impl
void
Scene::SaveToFile(const Path& scenePath) {}

void
Scene::AddModel(Uid modelResourceId, const String& modelName, const entt::entity& parent) {
  DLOG_ASSERT(Entity::HasComponent<HierarchyComponent>(this, parent));

  auto modelEntity =
      Entity::CreateEntity<ModelEntityPolicy>(this, modelResourceId, m_resourceManager);
  auto& modelTagComponent = Entity::GetComponent<UniqueTagComponent>(this, modelEntity);

  modelTagComponent.tagName = modelName;

  HierarchyComponent::AddChild(parent, m_world, modelEntity);
}

void
Scene::AddBillBoard(Uid texture2DResourceId, glm::vec3 point, const entt::entity& parent) {
  DLOG_ASSERT(Entity::HasComponent<HierarchyComponent>(this, parent));

  auto billBoardEntity = Entity::CreateEntity<BillBoardPolicy>(this, texture2DResourceId);
  HierarchyComponent::AddChild(parent, m_world, billBoardEntity);
}

void
Scene::AddLight(LightType type, glm::vec3 point, const entt::entity& parent) {
  DLOG_ASSERT(Entity::HasComponent<HierarchyComponent>(this, parent));

  auto lightEntity = Entity::CreateEntity<LightPolicy>(this, type, point);
  HierarchyComponent::AddChild(parent, m_world, lightEntity);
}

}  // namespace Marbas
