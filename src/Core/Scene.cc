#include "Core/Sence.h"
#include "Core/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glog/logging.h>

namespace Marbas {

std::unique_ptr<Scene> Scene::CreateSceneFromFile(const Path &sceneFile) {
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

    // TODO: process model(mesh) for the scene

    auto aRootNode = assimpScene->mRootNode;

    // NOTE:A root node has at least one child node
    for(int i = 0; i < aRootNode->mNumChildren; i++) { 
        auto childNode = aRootNode->mChildren[i];

        // ProcessNode(rootNode, rootNode, assimpScene, childNode);
    }

    return nullptr;
}

}  // namespace Marbas
