#ifndef MARBAS_CORE_SCENE_H
#define MARBAS_CORE_SCENE_H

#include "Common.h"
#include "Core/Model.h"

namespace Marbas {

class SceneNode {
public:
    explicit SceneNode(const String& nodeName) :
        m_sceneNodeName(nodeName)
    {}
    virtual ~SceneNode() = default;

public:
    const char* GetSceneNodeName() {
        return m_sceneNodeName.c_str();
    }

    SceneNode** GetSubSceneNodes() {
        return m_subSceneNode.data();
    }

    size_t GetSubSceneNodesCount() {
        return m_subSceneNode.size();
    }

    void AddSubSceneNode(SceneNode* node) {
        m_subSceneNode.push_back(node);
    }

    void AddMesh(std::unique_ptr<Mesh>&& mesh) {
        m_mesh.push_back(std::move(mesh));
    }

    size_t GetMeshCount() {
        return m_mesh.size();
    }

protected:
    String m_sceneNodeName;
    Vector<SceneNode*> m_subSceneNode;
    Vector<std::unique_ptr<Mesh>> m_mesh;
};

class SceneLight : public SceneNode {
};

/**
 * @brief 
 *
 * @note a scene tree's root node always has three child node, which are light node, model node, and
 *       camera node;
 */
class Scene {
public:
    Scene() {
        m_allSceneNode.push_back(std::make_unique<SceneNode>("RootNode"));
        m_rootNode = m_allSceneNode[0].get();
    }

    ~Scene() = default;

public:
    void AddLight();

    void AddCamera();

    void RegisterSceneNode(std::unique_ptr<SceneNode>&& sceneNode) {
        m_allSceneNode.push_back(std::move(sceneNode));
    }

public:

    // TODO: need to implement

    /**
     * @brief Create the Scene Tree from the scene file(such glTF)
     *
     * @param sceneFile scene file which needs to be supported bu assimp
     *
     * @return Scene
     */
    static std::unique_ptr<Scene> CreateSceneFromFile(const Path& sceneFile);

    [[nodiscard]] SceneNode* GetRootSceneNode() const noexcept {
        return m_rootNode;
    }

private:
    Vector<std::unique_ptr<SceneNode>> m_allSceneNode;

    SceneNode* m_rootNode;
};

}  // namespace Marbas

#endif
