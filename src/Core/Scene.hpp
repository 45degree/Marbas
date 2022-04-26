#ifndef MARBAS_CORE_SCENE_H
#define MARBAS_CORE_SCENE_H

#include "Common.hpp"
#include "RHI/RHI.hpp"
#include "entt/entt.hpp"
#include "Core/Mesh.hpp"

namespace Marbas {

class Scene;
class SceneNode {
    friend Scene;

    /**
     * @brief Create a scene node
     *
     * @param[in] nodeName the node's name
     * @param[in] isStatic whether the node is a statis node, 
     *            all static node in scnen will in one
     * drawcall
     */
    explicit SceneNode(const String& nodeName) :
        m_sceneNodeName(nodeName)
    {}
    virtual ~SceneNode() = default;

public:
    [[nodiscard]] const char* GetSceneNodeName() const {
        return m_sceneNodeName.c_str();
    }

    [[nodiscard]] Vector<const SceneNode*> GetSubSceneNodes() const {
        Vector<const SceneNode*> result;
        std::transform(m_subSceneNode.begin(), m_subSceneNode.end(), std::back_inserter(result),
            [](const std::unique_ptr<SceneNode>& node)->const SceneNode* { return node.get(); }
        );

        return result;
    }

    void AddSubSceneNode(std::unique_ptr<SceneNode>&& node) {
        m_subSceneNode.push_back(std::move(node));
    }

    [[nodiscard]] const Vector<Mesh>& GetMeshes() const noexcept {
        return m_meshes;
    }

    void DeleteSubSceneNode(const SceneNode* node) {
        for(int i = 0; i < m_subSceneNode.size(); i++) {
            if(node == m_subSceneNode[i].get()) {
                m_subSceneNode.erase(m_subSceneNode.begin() + i);
                return;
            }
        }
        return;
    }

protected:
    String m_sceneNodeName;
    Vector<Mesh> m_meshes;
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
    Scene();

    ~Scene() = default;

public:
    void AddLight();

    void AddCamera();

    [[nodiscard]] SceneNode* GetRootSceneNode() const noexcept {
        return m_rootNode.get();
    }

    void DeleteSceneNode(SceneNode* sceneNode);

    entt::registry& GetRigister() {
        return m_registry;
    }

public:

    void CombineStaticEntity();

    /**
     * @brief collection all static node and push them in a drawCollection;
     */
    void GenarateStaticRenderDate();

public:

    /**
     * @brief Create the Scene Tree from the scene file(such glTF)
     *
     * @param sceneFile scene file which needs to be supported bu assimp
     *
     * @return Scene
     */
    static std::unique_ptr<Scene> CreateSceneFromFile(const Path& sceneFile);

private:

    std::unique_ptr<SceneNode> m_rootNode = nullptr;

    // TODO: new
    entt::registry m_registry;
};

}  // namespace Marbas

#endif
