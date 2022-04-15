#include "Core/Scene.hpp"
#include "Core/Application.hpp"
#include "Tool/EncodingConvert.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glog/logging.h>

namespace Marbas {

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
        auto drawBatch = Application::GetRendererFactory()->CreateDrawBatch();
        auto material = std::make_unique<Material>();
        drawBatch->SetMaterial(material.get());

        for(int i = 0; i < aNode->mNumMeshes; i++) {
            auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];
            auto mesh = std::make_unique<Mesh>(path.string());
            mesh->SetMeshName(aMesh->mName.C_Str());
            mesh->ReadFromNode(aScene->mMeshes[aNode->mMeshes[i]], aScene);
            mesh->AddTexturesToMaterial(material.get());

            sceneNode->AddMesh(std::move(mesh));
        }

        sceneNode->SetDrawBatch(std::move(drawBatch));
        sceneNode->SetMaterial(std::move(material));
        sceneNode->GenerateGPUData();
    }

    for(int i = 0; i < aNode->mNumChildren; i++) {
        auto childNode = std::make_unique<SceneNode>(aNode->mChildren[i]->mName.C_Str());
        auto childNode_ptr = childNode.get();
        sceneNode->AddSubSceneNode(std::move(childNode));
        ProcessNode(scene, childNode_ptr, aScene, aNode->mChildren[i], path);
    }
}

void SceneNode::GenerateGPUData() {
    /**
     * TODO: the material can't add any texture after this function called
     */

    // calculate the size of the vertex data
    size_t totalVertexSize = std::accumulate(m_mesh.begin(), m_mesh.end(), 0, 
        [](size_t size, const std::unique_ptr<Mesh>& mesh){
            return size + mesh->GetVertexByte();
        }
    );
    size_t totalIndexCount = std::accumulate(m_mesh.begin(), m_mesh.end(), 0,
        [](size_t count, const std::unique_ptr<Mesh>& mesh) {
            return count + mesh->GetIndicesCount();
        }
    );
    auto vertexBuffer = Application::GetRendererFactory()->CreateVertexBuffer(totalVertexSize);
    auto indexBuffer = Application::GetRendererFactory()->CreateIndexBuffer(totalIndexCount);

    size_t vertexoOffset = 0, indexOffset = 0;
    for(auto& mesh : m_mesh) {
        auto vertices = mesh->GetVertices();
        auto indices = mesh->GetIndices();
        std::transform(indices.begin(), indices.end(), indices.begin(),
            [&indexOffset](uint32_t index) {
                return index + indexOffset;
            }
        );

        // TODO: need to file the texture id
        auto diffuseId = m_material->GetTextureBindPoint(mesh->GetDiffuseTexture());
        auto ambientId = m_material->GetTextureBindPoint(mesh->GetAmbientTexture());
        for(auto& vertex : vertices) {
            vertex.diffuseTextureId = diffuseId;
            vertex.ambientTextureId = ambientId;
        }

        vertexBuffer->SetData(vertices.data(), mesh->GetVertexByte(), vertexoOffset);
        indexBuffer->SetData(indices, indexOffset);
        vertexoOffset += mesh->GetVertexByte();
        indexOffset += indices.size();
    }
    vertexBuffer->SetLayout(GetMeshVertexInfoLayout());
    auto vertexArray = Application::GetRendererFactory()->CreateVertexArray();

    m_drawBatch->SetVertexArray(std::move(vertexArray));
    m_drawBatch->SetVertexBuffer(std::move(vertexBuffer));
    m_drawBatch->SetIndexBuffer(std::move(indexBuffer));
}

std::unique_ptr<Scene> Scene::CreateSceneFromFile(const Path& sceneFile) {

    Assimp::Importer importer;

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
