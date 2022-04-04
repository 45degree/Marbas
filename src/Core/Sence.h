#ifndef MARBAS_CORE_SCENE_H
#define MARBAS_CORE_SCENE_H

#include "Common.h"
#include "Core/Model.h"

namespace Marbas {

class SceneNode {
public:
    explicit SceneNode(const String& nodeName = "SceneNode") :
        m_sceneNodeName(nodeName)
    {}
    ~SceneNode() = default;

public:
    const char* GetSceneNodeName() {
        return m_sceneNodeName.c_str();
    }

protected:
    String m_sceneNodeName;
};

/**
 * @brief the scene node which implement this interface means it can add subnode
 */
class IAddSubNode {
public:
    virtual void AddSubSceneNode(const SceneNode* sceneNode) = 0;
};


/**
 * @brief a collection node for the scene node, it only stored a node name, you can add a subnode
 *        in it
 */
class CollectionSceneNode : public SceneNode, IAddSubNode {
public:
    explicit CollectionSceneNode(const String& nodeName = "SceneNode") :
        SceneNode(nodeName)
    {}
    ~CollectionSceneNode() = default;

public:
    void AddSubSceneNode(const SceneNode* sceneNode) override {
        m_subnode.push_back(sceneNode);
    }

protected:
    Vector<const SceneNode*> m_subnode;

};

/**
 * @brief a mesh node is a node which contains a single mesh
 */
class MeshNodeLeaf final : public SceneNode {
public:
    explicit MeshNodeLeaf(const String& nodeName) : SceneNode(nodeName)
    {}
    ~MeshNodeLeaf() = default;

public:
    void SetMesh(std::unique_ptr<Mesh>&& mesh) {
        m_mesh = std::move(mesh);
    }

private:
    std::unique_ptr<Mesh> m_mesh;
};

/**
 * @brief a model node is a node which cantains a single model
 *
 * @note A model node can contain multiple mesh nodes, and multiple mesh nodes can also be
 *       combined into one model node
 */
class ModelNodeLeaf final : public SceneNode {
public:

    // TODO: need to implement
    /**
     * @brief 
     *
     * @param collectionNode
     */
    static std::unique_ptr<ModelNodeLeaf>
    ConvertNodeToModelNode(std::unique_ptr<IAddSubNode>&& collectionNode);

    // TODO: need to implement
    /**
     * @brief 
     *
     * @param node
     *
     * @return 
     */
    static bool CheckNodeCanBeConvertToModelNode(const IAddSubNode* node);

private:
    Vector<const MeshNodeLeaf*> m_meshs;
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
        m_allSceneNode.push_back(std::make_unique<SceneNode>());
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

private:

    [[nodiscard]] SceneNode* GetRootSceneNode() const noexcept {
        return m_rootNode;
    }

private:
    Vector<std::unique_ptr<SceneNode>> m_allSceneNode;

    SceneNode* m_rootNode;
};

}  // namespace Marbas

#endif
