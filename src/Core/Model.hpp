#ifndef MARBAS_CORE_MODEL_H
#define MARBAS_CORE_MODEL_H

#include "Common.hpp"
#include "Core/Mesh.hpp"
#include "Core/Camera.hpp"
#include "RHI/RHI.hpp"

#include <assimp/scene.h>

namespace Marbas {

class Model {
public:
    Model();
    ~Model();

public:
    void CreateFromFile(const Path& sceneFile);

    void CreateFromAssimp(const aiScene* scene, const aiNode* node);

    void SetPosition(const glm::vec3& newPos) {
        m_position = newPos;
    }

    [[nodiscard]] glm::mat4 GetModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0);
        return glm::translate(model, m_position);
    }

    void Draw();

    void GenerateGPUData() {
        for(auto& mesh : m_meshes) {
            mesh->LoadToGPU();
        }
    }

    void DeleteGPUData() {
        for(auto& mesh : m_meshes) {
            mesh->UnLoadFromGPU();
        }
    }

    DrawCollection* GetDrawCollection() {
        return m_drawCollection.get();
    }

private:
    void ProcessNode(const aiScene* scene, const aiNode* node);

private:
    Path m_scenePath;
    Vector<std::unique_ptr<Mesh>> m_meshes;
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    std::unique_ptr<DrawCollection> m_drawCollection;
};

}  // namespace Marbas

#endif
