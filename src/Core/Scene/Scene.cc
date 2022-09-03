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
  // create a light
  auto light = LightPolicy::Create(m_world, LightType::PointLight);
  auto& component = Entity::GetComponent<PointLightComponent>(this, light);
  component.m_light.SetPos(glm::vec3(0, 30, 0));
  component.m_light.SetColor(glm::vec3(1, 1, 1));
  // component.m_light.SetDirection(glm::normalize(glm::vec3(0, -10, -5)));
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
  DLOG_ASSERT(Entity::HasComponent<HierarchyComponent>(this, parent))
      << "can't find the HierarchyComponent from the entity";

  auto& hierarchyComponent = Entity::GetComponent<HierarchyComponent>(this, parent);
  auto modelEntity =
      Entity::CreateEntity<ModelEntityPolicy>(this, modelResourceId, m_resourceManager);
  auto& modelTagComponent = Entity::GetComponent<UniqueTagComponent>(this, modelEntity);

  modelTagComponent.tagName = modelName;

  hierarchyComponent.children.push_back(modelEntity);
}

void
Scene::AddBillBoard(Uid texture2DResourceId, glm::vec3 point, const entt::entity& parent) {
  DLOG_ASSERT(Entity::HasComponent<HierarchyComponent>(this, parent))
      << "can't find the HierarchyComponent from the entity";

  auto& hierarchyComponent = Entity::GetComponent<HierarchyComponent>(this, parent);
  auto billBoardEntity = Entity::CreateEntity<BillBoardPolicy>(this, texture2DResourceId);
  auto& billBoardTagComponent = Entity::GetComponent<UniqueTagComponent>(this, billBoardEntity);

  HierarchyComponent::AddChild(parent, m_world, billBoardEntity);
}

// static bool DeleteNode(SceneNode* sceneNode) {
//   auto subNodes = sceneNode->GetSubSceneNodes();
//   for (const auto* subNode : subNodes) {
//     if (DeleteNode(const_cast<SceneNode*>(subNode))) {
//       sceneNode->DeleteSubSceneNode(subNode);
//     }
//   }
//
//   return sceneNode->GetSubSceneNodes().empty() && sceneNode->GetMeshesCount() == 0;
// }

// static void ProcessSubNode(SceneNode* node, ResourceManager* resourceManager, const Path& path,
//                            const aiScene* aScene, const aiNode* aNode) {
//   if (node == nullptr) {
//     throw std::runtime_error("scene is null or lastSceneNode is null");
//   }
//
//   if (aNode->mNumMeshes > 0) {
//     for (int i = 0; i < aNode->mNumMeshes; i++) {
//       auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];
//       auto* scene = const_cast<Scene*>(node->GetScene());
//
//       auto mesh = Entity::CreateEntity<MeshPolicy>(scene);
//
//       auto& tagsComponent = Entity::GetComponent<TagsCompoment>(scene, mesh);
//       tagsComponent.name = String(aMesh->mName.C_Str());
//
//       auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, mesh);
//       MeshPolicy::ReadVertexFromNode(aMesh, aScene, meshComponent);
//
//       if (Entity::HasComponent<RenderComponent>(scene, mesh)) {
//         auto& renderComponent = Entity::GetComponent<RenderComponent>(scene, mesh);
//         MeshPolicy::ReadMaterialFromNode(aMesh, aScene, path, renderComponent, resourceManager);
//       }
//
//       node->AddEntity(mesh);
//     }
//   }
//
//   for (int i = 0; i < aNode->mNumChildren; i++) {
//     ProcessSubNode(node, resourceManager, path, aScene, aNode->mChildren[i]);
//   }
// }
//
// std::unique_ptr<SceneNode> SceneNode::ReadModelFromFile(const Path& filePath, Scene* scene,
//                                                         ResourceManager* resourceManager) {
//   Assimp::Importer importer;
//
//   auto filename = filePath.filename().string();
//
//   const auto pos = filename.find_last_of('.');
//   const auto modelName = filename.substr(0, pos);
//
//   const auto* assimpScene =
//       importer.ReadFile(filePath.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
//
//   if (assimpScene == nullptr) {
//     auto errorStr = String(importer.GetErrorString());
//     LOG(ERROR) << FORMAT("can't load scene because: {}", errorStr);
//     return nullptr;
//   }
//
//   auto aRootNode = assimpScene->mRootNode;
//   auto modelSceneNode = std::make_unique<SceneNode>(scene, modelName.c_str());
//
//   ProcessSubNode(modelSceneNode.get(), resourceManager, filePath.parent_path(), assimpScene,
//                  aRootNode);
//
//   return modelSceneNode;
// }
//
// void Scene::CombineStaticEntity() {
//   const auto& entities = m_registry.view<MeshComponent, StaticMeshComponent>();
//   for (const auto& entity : entities) {
//     auto& mesh = entities.get<MeshComponent>(entity);
//   }
// }
//
// Scene::Scene(const Path& path, ResourceManager* resourceManager)
//     : m_rootNode(std::make_unique<SceneNode>(this, "RootNode")),
//       m_path(path),
//       m_resourceManager(resourceManager) {
//   m_skybox = CubeMapPolicy::Create(m_registry);
//   auto& component = Entity::GetComponent<CubeMapComponent>(this, m_skybox);
//
//   CubeMapCreateInfo defaultCreateInfo = {
//       .top = Path("assert/skybox/top.jpg"),
//       .bottom = Path("assert/skybox/bottom.jpg"),
//       .back = Path("assert/skybox/back.jpg"),
//       .front = Path("assert/skybox/front.jpg"),
//       .left = Path("assert/skybox/left.jpg"),
//       .right = Path("assert/skybox/right.jpg"),
//   };
//   component.m_cubeMapResource = m_resourceManager->AddCubeMap(defaultCreateInfo);
//   component.m_materialResource = m_resourceManager->AddMaterial();
//   auto shader = m_resourceManager->GetDefaultCubeMapShader();
//   component.m_materialResource->SetShader(shader);
// }
//
// void Scene::DeleteSceneNode(SceneNode* sceneNode) {
//   std::function<const SceneNode*(const SceneNode*, const SceneNode*)> findParent =
//       [&, this](const SceneNode* sceneNode, const SceneNode* currentNode) -> const SceneNode* {
//     if (this->m_rootNode.get() == sceneNode) {
//       return nullptr;
//     }
//
//     auto subNodes = currentNode->GetSubSceneNodes();
//     for (const auto* subNode : subNodes) {
//       if (subNode == sceneNode) {
//         return currentNode;
//       }
//
//       auto parent = findParent(sceneNode, subNode);
//       if (parent != nullptr) {
//         return parent;
//       }
//     }
//     return nullptr;
//   };
//
//   auto parent = const_cast<SceneNode*>(findParent(sceneNode, m_rootNode.get()));
//   if (parent == nullptr) return;
//
//   parent->DeleteSubSceneNode(sceneNode);
// }

}  // namespace Marbas
