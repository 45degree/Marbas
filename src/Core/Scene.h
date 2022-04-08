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
    [[nodiscard]] const char* GetSceneNodeName() const {
        return m_sceneNodeName.c_str();
    }

    [[nodiscard]] Vector<const SceneNode*> GetSubSceneNodes() const {
        // return m_subSceneNode.data();
        Vector<const SceneNode*> result;
        for(auto& subNode : m_subSceneNode) {
            result.push_back(subNode.get());
        }
        return result;
    }

    void AddSubSceneNode(std::unique_ptr<SceneNode>&& node) {
        m_subSceneNode.push_back(std::move(node));
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

    [[nodiscard]] size_t GetMeshCount() const {
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
    Vector<std::unique_ptr<SceneNode>> m_subSceneNode;
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
        m_rootNode = std::make_unique<SceneNode>("RootNode");
    }

    ~Scene() = default;

public:
    void AddLight();

    void AddCamera();

    [[nodiscard]] SceneNode* GetRootSceneNode() const noexcept {
        return m_rootNode.get();
    }

    void DeleteSceneNode(SceneNode* sceneNode);

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
    // Vector<std::unique_ptr<SceneNode>> m_allSceneNode;

    std::unique_ptr<SceneNode> m_rootNode = nullptr;
};

}  // namespace Marbas

#endif
