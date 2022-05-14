#include "Core/Scene.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <algorithm>
#include <assimp/Importer.hpp>

#include "Core/Application.hpp"
#include "Core/Component.hpp"
#include "Core/Entity.hpp"
#include "Core/Mesh.hpp"
#include "Tool/EncodingConvert.hpp"

namespace Marbas {

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

static void ProcessSubNode(SceneNode* node, ResourceManager* resourceManager, const Path& path,
                           const aiScene* aScene, const aiNode* aNode) {
  if (node == nullptr) {
    throw std::runtime_error("scene is null or lastSceneNode is null");
  }

  if (aNode->mNumMeshes > 0) {
    for (int i = 0; i < aNode->mNumMeshes; i++) {
      auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];
      auto* scene = const_cast<Scene*>(node->GetScene());

      auto mesh = Entity::CreateEntity<MeshPolicy>(scene);

      auto& tagsComponent = Entity::GetComponent<TagsCompoment>(scene, mesh);
      tagsComponent.name = String(aMesh->mName.C_Str());

      auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, mesh);
      MeshPolicy::ReadVertexFromNode(aMesh, aScene, meshComponent);

      if (Entity::HasComponent<RenderComponent>(scene, mesh)) {
        auto& renderComponent = Entity::GetComponent<RenderComponent>(scene, mesh);
        MeshPolicy::ReadMaterialFromNode(aMesh, aScene, path, renderComponent, resourceManager);
      }

      node->AddEntity(mesh);
    }
  }

  for (int i = 0; i < aNode->mNumChildren; i++) {
    ProcessSubNode(node, resourceManager, path, aScene, aNode->mChildren[i]);
  }
}

std::unique_ptr<SceneNode> SceneNode::ReadModelFromFile(const Path& filePath, Scene* scene,
                                                        ResourceManager* resourceManager) {
  Assimp::Importer importer;

  auto filename = filePath.filename().string();

  const auto pos = filename.find_last_of('.');
  const auto modelName = filename.substr(0, pos);

  const auto* assimpScene =
      importer.ReadFile(filePath.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

  if (assimpScene == nullptr) {
    auto errorStr = String(importer.GetErrorString());
    LOG(ERROR) << FORMAT("can't load scene because: {}", errorStr);
    return nullptr;
  }

  auto aRootNode = assimpScene->mRootNode;
  auto modelSceneNode = std::make_unique<SceneNode>(scene, modelName.c_str());

  ProcessSubNode(modelSceneNode.get(), resourceManager, filePath.parent_path(), assimpScene,
                 aRootNode);

  return modelSceneNode;
}

void Scene::CombineStaticEntity() {
  const auto& entities = m_registry.view<MeshComponent, StaticMeshComponent>();
  for (const auto& entity : entities) {
    auto& mesh = entities.get<MeshComponent>(entity);
  }
}

Scene::Scene(const Path& path, ResourceManager* resourceManager)
    : m_rootNode(std::make_unique<SceneNode>(this, "RootNode")),
      m_path(path),
      m_resourceManager(resourceManager) {
  m_skybox = CubeMapPolicy::Create(m_registry);
  auto& component = Entity::GetComponent<CubeMapComponent>(this, m_skybox);

  CubeMapCreateInfo defaultCreateInfo = {
      .top = Path("assert/skybox/top.jpg"),
      .bottom = Path("assert/skybox/bottom.jpg"),
      .back = Path("assert/skybox/back.jpg"),
      .front = Path("assert/skybox/front.jpg"),
      .left = Path("assert/skybox/left.jpg"),
      .right = Path("assert/skybox/right.jpg"),
  };
  component.m_cubeMapResource = m_resourceManager->AddCubeMap(defaultCreateInfo);
  component.m_materialResource = m_resourceManager->AddMaterial();
  auto shader = m_resourceManager->GetDefaultCubeMapShader();
  component.m_materialResource->SetShader(shader);
}

void Scene::DeleteSceneNode(SceneNode* sceneNode) {
  std::function<const SceneNode*(const SceneNode*, const SceneNode*)> findParent =
      [&, this](const SceneNode* sceneNode, const SceneNode* currentNode) -> const SceneNode* {
    if (this->m_rootNode.get() == sceneNode) {
      return nullptr;
    }

    auto subNodes = currentNode->GetSubSceneNodes();
    for (const auto* subNode : subNodes) {
      if (subNode == sceneNode) {
        return currentNode;
      }

      auto parent = findParent(sceneNode, subNode);
      if (parent != nullptr) {
        return parent;
      }
    }
    return nullptr;
  };

  auto parent = const_cast<SceneNode*>(findParent(sceneNode, m_rootNode.get()));
  if (parent == nullptr) return;

  parent->DeleteSubSceneNode(sceneNode);
}

}  // namespace Marbas
