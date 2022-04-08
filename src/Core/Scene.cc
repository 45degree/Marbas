#include "Core/Scene.h"
#include "Core/Model.h"
#include "Core/Application.h"
#include "Tool/EncodingConvert.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/DefaultIOSystem.h>
#include <assimp/DefaultIOStream.h>
#include <assimp/ai_assert.h>

#include <glog/logging.h>


namespace Marbas {

class MyIoSystem;
class MyIOStream : public Assimp::DefaultIOStream {
    friend MyIoSystem;
protected:
    MyIOStream(FILE* file, const char* strFile) : DefaultIOStream(file, strFile) {}
};

class MyIoSystem : public Assimp::DefaultIOSystem {
public:
    bool Exists( const char* pFile) const override {
        FILE *file = ::fopen(pFile, "rb");
        if (!file) {
            return false;
        }

        ::fclose(file);
        return true;
    }

    Assimp::IOStream* Open(const char *strFile, const char *strMode) override {
        ai_assert(strFile != nullptr);
        ai_assert(strMode != nullptr);
        FILE *file;
        file = ::fopen(strFile, strMode);
        if (!file) {
            return nullptr;
        }

        return new MyIOStream(file, strFile);
    }
};

static bool DeleteNode(SceneNode* sceneNode) {
    auto subNodes = sceneNode->GetSubSceneNodes();
    for(const auto* subNode : subNodes) {
        if(DeleteNode(const_cast<SceneNode*>(subNode))) {
            sceneNode->DeleteSubSceneNode(subNode);
        }
    }

    return sceneNode->GetSubSceneNodes().empty() && sceneNode->GetMeshCount() == 0;
}

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
            mesh->SetMeshName(aMesh->mName.C_Str());
            mesh->ReadFromNode(aScene->mMeshes[aNode->mMeshes[i]], aScene);

            drawCollection->AddDrawUnit(mesh->GetDrawUnit());
            sceneNode->AddMesh(std::move(mesh));
        }
        sceneNode->SetDrawCollection(std::move(drawCollection));
    }

    for(int i = 0; i < aNode->mNumChildren; i++) {
        auto childNode = std::make_unique<SceneNode>(aNode->mChildren[i]->mName.C_Str());
        auto childNode_ptr = childNode.get();
        sceneNode->AddSubSceneNode(std::move(childNode));
        ProcessNode(scene, childNode_ptr, aScene, aNode->mChildren[i], path);
    }
}

std::unique_ptr<Scene> Scene::CreateSceneFromFile(const Path& sceneFile) {

    Assimp::Importer importer;
    importer.SetIOHandler(new MyIoSystem());

    auto filename = sceneFile.string();
    const auto* assimpScene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate |
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
    DeleteNode(rootNode);

    return scene;
}

void Scene::DeleteSceneNode(SceneNode *sceneNode) {
    std::function<const SceneNode*(const SceneNode*, const SceneNode*)> findParent =
        [&, this](const SceneNode* sceneNode, const SceneNode* currentNode) -> const SceneNode* {
        if(this->m_rootNode.get() == sceneNode) {
            return nullptr;
        }

        auto subNodes = currentNode->GetSubSceneNodes();
        for(const auto* subNode : subNodes) {
            if(subNode == sceneNode) {
                return currentNode;
            }

            auto parent = findParent(sceneNode, subNode);
            if(parent != nullptr) {
                return parent;
            }
        }
        return nullptr;
    };

    auto parent = const_cast<SceneNode*>(findParent(sceneNode, m_rootNode.get()));
    if(parent == nullptr) return;

    parent->DeleteSubSceneNode(sceneNode);
}

}  // namespace Marbas
