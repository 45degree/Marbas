#include "Core/Scene.h"
#include "Core/Model.h"
#include "Core/Application.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glog/logging.h>

namespace Marbas {

static void ProcessNode(Scene* scene, SceneNode* sceneNode,
                        const aiScene* aScene, const aiNode* aNode, const Path& path)
{
    if(scene == nullptr || sceneNode == nullptr) {
        throw std::runtime_error("scene is null or lastSceneNode is null");
    }

    if(aNode->mNumMeshes > 0) {
        auto drawCollection = Application::GetRendererFactory()->CreateDrawCollection();
        for(int i = 0; i < aNode->mNumMeshes; i++) {
            auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];
            auto mesh = std::make_unique<Mesh>(path.string());
            mesh->ReadFromNode(aScene->mMeshes[aNode->mMeshes[i]], aScene);

            drawCollection->AddDrawUnit(mesh->GetDrawUnit());
            sceneNode->AddMesh(std::move(mesh));
        }
        sceneNode->SetDrawCollection(std::move(drawCollection));
    }

    for(int i = 0; i < aNode->mNumChildren; i++) {
        auto childNode = std::make_unique<SceneNode>(aNode->mChildren[i]->mName.C_Str());
        auto childNode_ptr = childNode.get();
        sceneNode->AddSubSceneNode(childNode.get());
        scene->RegisterSceneNode(std::move(childNode));
        ProcessNode(scene, childNode_ptr, aScene, aNode->mChildren[i], path);
    }
}

std::unique_ptr<Scene> Scene::CreateSceneFromFile(const Path& sceneFile) {

    Assimp::Importer importer;
    auto filename = sceneFile.string();
    const auto* assimpScene = importer.ReadFile(filename, aiProcess_Triangulate |
                                                          aiProcess_FlipUVs);
    if (assimpScene == nullptr) {
        auto errorStr = String(importer.GetErrorString());
        LOG(ERROR) << FORMAT("can't load scene because: {}", errorStr);
        return nullptr;
    }

    auto scene = std::make_unique<Scene>();
    auto rootNode = scene->GetRootSceneNode();

    if(assimpScene->HasLights()) {
        for(int i = 0; i < assimpScene->mNumLights; i++) {
            auto light = assimpScene->mLights[i];

            // TODO: add light to the scene
        }
    }

    if(assimpScene->HasCameras()) {
        for(int i = 0; i < assimpScene->mNumCameras; i++) {
            auto camera = assimpScene->mCameras[i];

            // TODO: add camera to the scene
        }
    }

    auto aRootNode = assimpScene->mRootNode;
    ProcessNode(scene.get(), rootNode, assimpScene, aRootNode, sceneFile.parent_path());

    return scene;
}

}  // namespace Marbas
