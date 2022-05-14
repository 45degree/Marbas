#ifndef MARBAS_CORE_SCENE_H
#define MARBAS_CORE_SCENE_H

#include "Common.hpp"
#include "Core/Mesh.hpp"
#include "RHI/RHI.hpp"
#include "entt/entt.hpp"

namespace Marbas {

class Scene;

class SceneNode {
  friend Scene;

 public:
  /**
   * @brief Create a scene node
   *
   * @param[in] nodeName the node's name
   * @param[in] isStatic whether the node is a statis node,
   *            all static node in scnen will in one
   * drawcall
   */
  explicit SceneNode(Scene* scene, const String& nodeName = "")
      : m_scene(scene), m_sceneNodeName(nodeName) {}
  virtual ~SceneNode() = default;

 public:
  [[nodiscard]] const char* GetSceneNodeName() const { return m_sceneNodeName.c_str(); }
  void SetSceneNodeName(const String& name) { m_sceneNodeName = name; }

  [[nodiscard]] Vector<const SceneNode*> GetSubSceneNodes() const {
    Vector<const SceneNode*> result;
    std::transform(
        m_subSceneNode.begin(), m_subSceneNode.end(), std::back_inserter(result),
        [](const std::unique_ptr<SceneNode>& node) -> const SceneNode* { return node.get(); });

    return result;
  }

  [[nodiscard]] const Scene* GetScene() const noexcept { return m_scene; }

  void AddSubSceneNode(std::unique_ptr<SceneNode>&& node) {
    m_subSceneNode.push_back(std::move(node));
  }

  void AddEntity(const entt::entity entity) { m_entity.push_back(entity); }

  [[nodiscard]] const Vector<entt::entity>& GetMeshes() const noexcept { return m_entity; }

  [[nodiscard]] size_t GetMeshesCount() const noexcept { return m_entity.size(); }

  void DeleteSubSceneNode(const SceneNode* node) {
    for (int i = 0; i < m_subSceneNode.size(); i++) {
      if (node == m_subSceneNode[i].get()) {
        m_subSceneNode.erase(m_subSceneNode.begin() + i);
        return;
      }
    }
    return;
  }

 public:
  /**
   * @brief Create the Mesh SceneNode from the model file(such as obj)
   *
   * @param filePath model file's path
   * @param scene
   * @param resourceManager
   *
   * @return
   */
  static std::unique_ptr<SceneNode> ReadModelFromFile(const Path& filePath, Scene* scene,
                                                      ResourceManager* resourceManager);

 protected:
  Scene* m_scene;
  String m_sceneNodeName;
  Vector<entt::entity> m_entity;
  Vector<std::unique_ptr<SceneNode>> m_subSceneNode;
};

/**
 * @brief
 *
 * @note a scene tree's root node always has three child node, which are light node, model node, and
 *       camera node;
 */
class Entity;
class Scene {
  friend Entity;

 public:
  Scene(const Path& path, ResourceManager* resourceManager);

  ~Scene() = default;

 public:
  void AddLight();

  void AddCamera();

  [[nodiscard]] SceneNode* GetRootSceneNode() const noexcept { return m_rootNode.get(); }

  void DeleteSceneNode(SceneNode* sceneNode);

  entt::registry& GetRigister() { return m_registry; }

  [[nodiscard]] const Path& GetPath() const { return m_path; }

 public:
  void CombineStaticEntity();

  /**
   * @brief collection all static node and push them in a drawCollection;
   */
  void GenarateStaticRenderDate();

  [[nodiscard]] CubeMap GetCubeMap() const noexcept { return m_cubeMap; }

 public:
  /**
   * @brief Create the Scene Tree from the scene file(such glTF)
   *
   * @param sceneFile scene file which needs to be supported bu assimp
   *
   * @return Scene
   */
  // static std::unique_ptr<Scene> CreateSceneFromFile(const Path& sceneFile,
  //                                                   ResourceManager* resourceManager);

 private:
  // void ProcessNode(SceneNode* sceneNode, ResourceManager* resourceManager, const aiScene* aScene,
  //                  const aiNode* aNode);

 private:
  std::unique_ptr<SceneNode> m_rootNode = nullptr;

  // TODO: new
  entt::registry m_registry;
  ResourceManager* m_resourceManager = nullptr;

  CubeMap m_cubeMap;

  const Path m_path;
  Vector<Mesh> m_staticMeshes;
};

}  // namespace Marbas

#endif
