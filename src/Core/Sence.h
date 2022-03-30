#ifndef MARBAS_CORE_SCENE_H
#define MARBAS_CORE_SCENE_H

#include "Common.h"
#include "Core/Model.h"

namespace Marbas {

class SceneNode {
public:
    explicit SceneNode(const String& nodeName, const SceneNode* rootNode = nullptr) :
        m_sceneNodeName(nodeName),
        m_rootNode(rootNode)
    {}
    ~SceneNode() = default;

public:
    const char* GetSceneNodeName() {
        return m_sceneNodeName.c_str();
    }

    void AddSubSceneNode(const SceneNode* sceneNode) {
        m_subSceneNode.push_back(sceneNode);
    }

protected:
    const SceneNode* m_rootNode;
    String m_sceneNodeName;
    Vector<const SceneNode*> m_subSceneNode;
};

class ModelNodeLeaf final : public SceneNode {
public:
    explicit ModelNodeLeaf(const String& modelName, const SceneNode* rootNode = nullptr):
        SceneNode(modelName, rootNode) {}
    ~ModelNodeLeaf() = default;

    ModelNodeLeaf(const ModelNodeLeaf&) = delete;
    ModelNodeLeaf& operator=(const ModelNodeLeaf&) = delete;

public:
    void SetModel(std::unique_ptr<Model>&& model) {
        m_model = std::move(model);
    }

    void SetModel(const Path& modelPath) {
        m_model = std::unique_ptr<Model>();
        m_model->ReadFromFile(modelPath);
    }

protected:
    std::unique_ptr<Model> m_model;
};

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

public:
    void AddModel(const Path& modelPath);

private:
    Vector<std::unique_ptr<SceneNode>> m_allSceneNode;

    SceneNode* m_rootNode;
};

}  // namespace Marbas

#endif
