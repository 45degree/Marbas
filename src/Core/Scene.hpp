#ifndef MARBAS_CORE_SCENE_H
#define MARBAS_CORE_SCENE_H

#include "Common.hpp"
#include "Core/Mesh.hpp"
#include "RHI/RHI.hpp"

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
    explicit SceneNode(const String& nodeName, bool isStatic = false) :
        m_sceneNodeName(nodeName),
        m_isStatic(isStatic)
    {}
    virtual ~SceneNode() = default;

public:
    [[nodiscard]] const char* GetSceneNodeName() const {
        return m_sceneNodeName.c_str();
    }

    /**
     * @brief Determine if a node is a static node
     *
     * @return true if the node is a static node
     */
    [[nodiscard]] bool IsStatic() const noexcept {
        return m_isStatic;
    }

    /**
     * @brief change the node's static attribute
     *
     * @param isStatic is static or not
     */
    void ChangeStatic(bool isStatic) noexcept {
        m_isStatic = isStatic;
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
        m_mesh.push_back(std::move(mesh));
    }

    [[nodiscard]] size_t GetMeshCount() const {
        return m_mesh.size();
    }

    [[nodiscard]] const Mesh* GetMesh(size_t index) const {
        return m_mesh[index].get();
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

        return m_modelMatrix;
    }

    void SetModelMatrix(const glm::mat4& modelMatrix) {
        m_modelMatrix = modelMatrix;
    }

    void AddMaterial(std::unique_ptr<Material>&& material) {
        m_materials.push_back(std::move(material));
    }

    void AddDrawBatch(std::unique_ptr<DrawBatch>&& drawBatch) {
        m_drawBatches.push_back(std::move(drawBatch));
    }

    [[nodiscard]] Vector<DrawBatch*> GetDrawBatches() const noexcept {
        Vector<DrawBatch*> drawBatches;
        std::transform(m_drawBatches.begin(), m_drawBatches.end(), std::back_inserter(drawBatches),
            [](const std::unique_ptr<DrawBatch>& drawBatch) {
                return drawBatch.get();
            }
        );
        return drawBatches;
    }

    [[nodiscard]] Vector<Material*> GetMaterials() const noexcept {
        Vector<Material*> materials;
        std::transform(m_materials.begin(), m_materials.end(), std::back_inserter(materials),
            [](const std::unique_ptr<Material>& material){
                return material.get();
            }
        );
        return materials;
    }

    void GenerateGPUData(RHIFactory* rhiFactory);

protected:
    String m_sceneNodeName;
    bool m_isStatic;

    /// This matrix only takes effect when this node has a grid
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    Vector<std::unique_ptr<SceneNode>> m_subSceneNode;
    Vector<std::unique_ptr<Mesh>> m_mesh;

    Vector<std::unique_ptr<Material>> m_materials;
    Vector<std::unique_ptr<DrawBatch>> m_drawBatches;
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
    Scene():
        m_rootNode(std::make_unique<SceneNode>("RootNode"))
    {}

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
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indicesBuffer;

    std::unique_ptr<SceneNode> m_rootNode = nullptr;

    Vector<std::unique_ptr<DrawBatch>> m_staticDrawBatch;
};

}  // namespace Marbas

#endif
