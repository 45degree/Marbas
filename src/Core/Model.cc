#include "Core/Model.h"
#include "Core/Application.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


namespace Marbas {

Model::Model() {
    m_drawCollection = Application::GetRendererFactory()->CreateDrawCollection();
}

Model::~Model() = default;

void Model::CreateFromFile(const Path &sceneFile) {
    Assimp::Importer importer;
    auto filename = sceneFile.string();
    const auto* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  {
        LOG(ERROR) << FORMAT("Failed to load model: {}", importer.GetErrorString());
        return;
    }
    m_scenePath = sceneFile.parent_path();

    ProcessNode(scene, scene->mRootNode);
}

void Model::CreateFromAssimp(const aiScene *scene, const aiNode *node) {
    ProcessNode(scene, node);
}

void Model::ProcessNode(const aiScene* scene, const aiNode* node) {
    for(auto i = 0; i < node->mNumMeshes; i++) {
        auto mesh = std::make_unique<Mesh>(m_scenePath);
        mesh->ReadFromNode(scene->mMeshes[node->mMeshes[i]] , scene);
        m_drawCollection->AddDrawUnit(mesh->GetDrawUnit());
        m_meshes.push_back(std::move(mesh));
    }

    for(auto i = 0; i < node->mNumChildren; i++) {
        ProcessNode(scene, node->mChildren[i]);
    }
}

void Model::Draw() {
    GenerateGPUData();
    // m_drawCollection->Draw();
}


}  // namespace Marbas
