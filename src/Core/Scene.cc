#include "Core/Scene.hpp"
#include "Core/Mesh.hpp"
#include "Core/Application.hpp"
#include "Core/Component.hpp"
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
        // auto drawBatch = Application::GetRendererFactory()->CreateDrawBatch();
        // auto material = std::make_unique<Material>();
        // drawBatch->SetMaterial(material.get());

        for(int i = 0; i < aNode->mNumMeshes; i++) {
            auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];
            auto mesh = std::make_unique<Mesh>(path.string());
            mesh->SetMeshName(aMesh->mName.C_Str());
            mesh->ReadFromNode(aScene->mMeshes[aNode->mMeshes[i]], aScene);
            // mesh->AddTexturesToMaterial(material.get());

            sceneNode->AddMesh(std::move(mesh));
        }

        // sceneNode->AddDrawBatch(std::move(drawBatch));
        // sceneNode->SetMaterial(std::move(material));
        sceneNode->GenerateGPUData(Application::GetRendererFactory());
    }

    for(int i = 0; i < aNode->mNumChildren; i++) {
        auto childNode = std::make_unique<SceneNode>(aNode->mChildren[i]->mName.C_Str());
        auto childNode_ptr = childNode.get();
        sceneNode->AddSubSceneNode(std::move(childNode));
        ProcessNode(scene, childNode_ptr, aScene, aNode->mChildren[i], path);
    }
}

void Scene::CombineStaticEntity() {
    const auto& entities = m_registry.view<MeshComponent, StaticMeshComponent>();
    for(const auto& entity : entities) {
        auto& mesh = entities.get<MeshComponent>(entity);
    }
}

Scene::Scene():
    m_rootNode(std::make_unique<SceneNode>("RootNode"))
{
    Mesh staticMesh(this);
}

void SceneNode::GenerateGPUData(RHIFactory* rhiFactory) {

    // TODO: splite the mesh by texture
    if(m_mesh.empty()) return;

    std::map<std::pair<Texture2D*, Texture2D*>, Vector<Mesh*>> textures;
    for(const auto& mesh : m_mesh) {
        auto ambientTexture = mesh->GetAmbientTexture();
        auto diffuseTexture = mesh->GetDiffuseTexture();

        auto texturePair = std::make_pair(ambientTexture, diffuseTexture);
        if(textures.find(texturePair) == textures.end()) {
            textures.insert({texturePair, Vector<Mesh*>()});
        }
        textures.at(texturePair).push_back(mesh.get());
    }

    for(auto& texture : textures) {
        auto meshes = texture.second;

        auto material = std::make_unique<Material>();
        auto drawBatch = rhiFactory->CreateDrawBatch();

        material->SetAmbientTexture(texture.first.first);
        material->SetDiffuseTexture(texture.first.second);
        drawBatch->SetMaterial(material.get());

        /**
         * TODO: the material can't add any texture after this function called
         */

        // calculate the size of the vertex data
        size_t totalVertexSize = std::accumulate(meshes.begin(), meshes.end(), 0,
            [](size_t size, const Mesh* mesh){
                return size + mesh->GetVertexByte();
            }
        );

        size_t totalIndexCount = std::accumulate(meshes.begin(), meshes.end(), 0,
            [](size_t count, const Mesh* mesh) {
                return count + mesh->GetIndicesCount();
            }
        );

        auto vertexBuffer = rhiFactory->CreateVertexBuffer(totalVertexSize);
        auto vertexArray = rhiFactory->CreateVertexArray();
        auto indexBuffer = rhiFactory->CreateIndexBuffer(totalIndexCount);

        size_t vertexoOffset = 0, indexOffset = 0;
        for(auto& mesh : meshes) {
            auto vertices = mesh->GetVertices();
            auto indices = mesh->GetIndices();
            std::transform(indices.begin(), indices.end(), indices.begin(),
                [&indexOffset](uint32_t index) {
                    return index + indexOffset;
                }
            );

            // TODO: need to file the texture id
            auto diffuseId = material->GetTextureBindPoint(mesh->GetDiffuseTexture());
            auto ambientId = material->GetTextureBindPoint(mesh->GetAmbientTexture());
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

        drawBatch->SetVertexBuffer(std::move(vertexBuffer));
        drawBatch->SetVertexArray(std::move(vertexArray));
        drawBatch->SetIndexBuffer(std::move(indexBuffer));

        m_drawBatches.push_back(std::move(drawBatch));
        m_materials.push_back(std::move(material));
    }
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
