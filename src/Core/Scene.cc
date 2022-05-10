#include "Core/Scene.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <assimp/Importer.hpp>

#include "Core/Application.hpp"
#include "Core/Component.hpp"
#include "Core/Entity.hpp"
#include "Core/Mesh.hpp"
#include "Tool/EncodingConvert.hpp"

namespace Marbas {

static bool DeleteNode(SceneNode* sceneNode) {
  auto subNodes = sceneNode->GetSubSceneNodes();
  for (const auto* subNode : subNodes) {
    if (DeleteNode(const_cast<SceneNode*>(subNode))) {
      sceneNode->DeleteSubSceneNode(subNode);
    }
  }

  return sceneNode->GetSubSceneNodes().empty() && sceneNode->GetMeshesCount() == 0;
}

void Scene::ProcessNode(SceneNode* sceneNode, ResourceManager* resourceManager,
                        const aiScene* aScene, const aiNode* aNode) {
  if (sceneNode == nullptr) {
    throw std::runtime_error("scene is null or lastSceneNode is null");
  }

  if (aNode->mNumMeshes > 0) {
    for (int i = 0; i < aNode->mNumMeshes; i++) {
      auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];

      auto mesh = Entity::CreateEntity<MeshPolicy>(this);

      auto& tagsComponent = Entity::GetComponent<TagsCompoment>(this, mesh);
      tagsComponent.tags[TagsKey::NAME] = String(aMesh->mName.C_Str());

      auto& meshComponent = Entity::GetComponent<MeshComponent>(this, mesh);
      MeshPolicy::ReadVertexFromNode(aMesh, aScene, meshComponent);

      if (Entity::HasComponent<RenderComponent>(this, mesh)) {
        auto& renderComponent = Entity::GetComponent<RenderComponent>(this, mesh);
        MeshPolicy::ReadMaterialFromNode(aMesh, aScene, m_path, renderComponent, resourceManager);
      }

      sceneNode->m_meshes.push_back(mesh);
    }
  }

  for (int i = 0; i < aNode->mNumChildren; i++) {
    auto childNode = std::make_unique<SceneNode>(aNode->mChildren[i]->mName.C_Str());
    auto childNode_ptr = childNode.get();
    sceneNode->AddSubSceneNode(std::move(childNode));
    ProcessNode(childNode_ptr, resourceManager, aScene, aNode->mChildren[i]);
  }
}

void Scene::CombineStaticEntity() {
  const auto& entities = m_registry.view<MeshComponent, StaticMeshComponent>();
  for (const auto& entity : entities) {
    auto& mesh = entities.get<MeshComponent>(entity);
  }
}

Scene::Scene(const Path& path, ResourceManager* resourceManager)
    : m_rootNode(std::make_unique<SceneNode>("RootNode")),
      m_path(path),
      m_resourceManager(resourceManager) {
  m_cubeMap = CubeMapPolicy::Create(m_registry);
  auto& componnent = Entity::GetComponent<CubeMapComponent>(this, m_cubeMap.value());

  CubeMapCreateInfo defaultCreateInfo = {
      .top = Path("assert/skybox/top.jpg"),
      .bottom = Path("assert/skybox/bottom.jpg"),
      .back = Path("assert/skybox/back.jpg"),
      .front = Path("assert/skybox/front.jpg"),
      .left = Path("assert/skybox/left.jpg"),
      .right = Path("assert/skybox/right.jpg"),
  };
  componnent.m_cubeMapResource = m_resourceManager->AddCubeMapMaterial(defaultCreateInfo)->GetUid();
}

std::unique_ptr<Scene> Scene::CreateSceneFromFile(const Path& sceneFile,
                                                  ResourceManager* resourceManager) {
  Assimp::Importer importer;

  auto filename = sceneFile.string();
  const auto* assimpScene =
      importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

  if (assimpScene == nullptr) {
    auto errorStr = String(importer.GetErrorString());
    LOG(ERROR) << FORMAT("can't load scene because: {}", errorStr);
    return nullptr;
  }

  auto scene = std::make_unique<Scene>(sceneFile.parent_path(), resourceManager);
  auto rootNode = scene->GetRootSceneNode();

  if (assimpScene->HasLights()) {
    for (int i = 0; i < assimpScene->mNumLights; i++) {
      auto light = assimpScene->mLights[i];

      // TODO: add light to the scene
    }
  }

  if (assimpScene->HasCameras()) {
    for (int i = 0; i < assimpScene->mNumCameras; i++) {
      auto camera = assimpScene->mCameras[i];

      // TODO: add camera to the scene
    }
  }

  auto aRootNode = assimpScene->mRootNode;
  scene->ProcessNode(rootNode, resourceManager, assimpScene, aRootNode);
  DeleteNode(rootNode);

  return scene;
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
