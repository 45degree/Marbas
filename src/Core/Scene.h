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
        if(m_drawCollection != nullptr) {
            m_drawCollection->AddDrawUnit(mesh->GetDrawUnit());
        }
        m_mesh.push_back(std::move(mesh));
    }

    void SetDrawCollection(std::unique_ptr<DrawCollection>&& drawCollection) {
        for(auto& mesh : m_mesh) {
            mesh->LoadToGPU();
            drawCollection->AddDrawUnit(mesh->GetDrawUnit());
        }

        m_drawCollection = std::move(drawCollection);
    }

    size_t GetMeshCount() {
        return m_mesh.size();
    }

    [[nodiscard]] const Mesh* GetMesh(size_t index) const {
        return m_mesh[index].get();
    }

    [[nodiscard]] const DrawCollection* GetDrawCollection() const {
        return m_drawCollection.get();
    }

    /**
     * @brief get the scene model matrix if the scene node has meshes;
     *
     * @note if the scene node don't have meshes, this method will return an identity matrix
     *
     * @return the model matrix
     */
    [[nodiscard]] glm::mat4 GetModelMatrix() const {
        if(m_mesh.empty()) return glm::mat4(1.0f);

        return modelMatrix;
    }

protected:
    String m_sceneNodeName;

    /// This matrix only takes effect when this node has a grid
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    Vector<SceneNode*> m_subSceneNode;
    Vector<std::unique_ptr<Mesh>> m_mesh;
    std::unique_ptr<DrawCollection> m_drawCollection = nullptr;
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

    [[nodiscard]] SceneNode* GetRootSceneNode() const noexcept {
        return m_rootNode;
    }

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
    Vector<std::unique_ptr<SceneNode>> m_allSceneNode;

    SceneNode* m_rootNode;
};

}  // namespace Marbas

#endif
