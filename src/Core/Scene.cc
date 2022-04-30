#include "Core/Scene.hpp"
#include "Core/Mesh.hpp"
#include "Core/Entity.hpp"
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

    return sceneNode->GetSubSceneNodes().empty() && sceneNode->GetMeshesCount() == 0;
}

void Scene::ProcessNode(SceneNode* sceneNode, ResourceManager* resourceManager,
                        const aiScene* aScene, const aiNode* aNode)
{
    if(sceneNode == nullptr) {
        throw std::runtime_error("scene is null or lastSceneNode is null");
    }

    if(aNode->mNumMeshes > 0) {

        for(int i = 0; i < aNode->mNumMeshes; i++) {
            auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];

            auto mesh = Entity::CreateEntity<MeshPolicy>(this);

            auto& tagsComponent = Entity::GetComponent<TagsCompoment>(this, mesh);
            tagsComponent.tags[TagsKey::NAME] = String(aMesh->mName.C_Str());

            auto& meshComponent = Entity::GetComponent<MeshComponent>(this, mesh);
            MeshPolicy::ReadVertexFromNode(aMesh, aScene, meshComponent);

            if(Entity::HasComponent<RenderComponent>(this, mesh)) {
                auto& renderComponent = Entity::GetComponent<RenderComponent>(this, mesh);
                MeshPolicy::ReadMaterialFromNode(aMesh, aScene, m_path, renderComponent,
                                                 resourceManager);
            }

            sceneNode->m_meshes.push_back(mesh);
        }
    }

    for(int i = 0; i < aNode->mNumChildren; i++) {
        auto childNode = std::make_unique<SceneNode>(aNode->mChildren[i]->mName.C_Str());
        auto childNode_ptr = childNode.get();
        sceneNode->AddSubSceneNode(std::move(childNode));
        ProcessNode(childNode_ptr, resourceManager, aScene, aNode->mChildren[i]);
    }
}

void Scene::CombineStaticEntity() {
    const auto& entities = m_registry.view<MeshComponent, StaticMeshComponent>();
    for(const auto& entity : entities) {
        auto& mesh = entities.get<MeshComponent>(entity);
    }
}

Scene::Scene(const Path& path, ResourceManager* resourceManager):
    m_rootNode(std::make_unique<SceneNode>("RootNode")),
    m_path(path),
    m_resourceManager(resourceManager)
{}

// void SceneNode::GenerateGPUData(RHIFactory* rhiFactory) {
//
//     // TODO: splite the mesh by texture
//     if(m_mesh.empty()) return;
//
//     std::map<std::pair<Texture2D*, Texture2D*>, Vector<Mesh*>> textures;
//     for(const auto& mesh : m_mesh) {
//         auto ambientTexture = mesh->GetAmbientTexture();
//         auto diffuseTexture = mesh->GetDiffuseTexture();
//
//         auto texturePair = std::make_pair(ambientTexture, diffuseTexture);
//         if(textures.find(texturePair) == textures.end()) {
//             textures.insert({texturePair, Vector<Mesh*>()});
//         }
//         textures.at(texturePair).push_back(mesh.get());
//     }
//
//     for(auto& texture : textures) {
//         auto meshes = texture.second;
//
//         auto material = std::make_unique<Material>();
//         auto drawBatch = rhiFactory->CreateDrawBatch();
//
//         material->SetAmbientTexture(texture.first.first);
//         material->SetDiffuseTexture(texture.first.second);
//         drawBatch->SetMaterial(material.get());
//
//         /**
//          * TODO: the material can't add any texture after this function called
//          */
//
//         // calculate the size of the vertex data
//         size_t totalVertexSize = std::accumulate(meshes.begin(), meshes.end(), 0,
//             [](size_t size, const Mesh* mesh){
//                 return size + mesh->GetVertexByte();
//             }
//         );
//
//         size_t totalIndexCount = std::accumulate(meshes.begin(), meshes.end(), 0,
//             [](size_t count, const Mesh* mesh) {
//                 return count + mesh->GetIndicesCount();
//             }
//         );
//
//         auto vertexBuffer = rhiFactory->CreateVertexBuffer(totalVertexSize);
//         auto vertexArray = rhiFactory->CreateVertexArray();
//         auto indexBuffer = rhiFactory->CreateIndexBuffer(totalIndexCount);
//
//         size_t vertexoOffset = 0, indexOffset = 0;
//         for(auto& mesh : meshes) {
//             auto vertices = mesh->GetVertices();
//             auto indices = mesh->GetIndices();
//             std::transform(indices.begin(), indices.end(), indices.begin(),
//                 [&indexOffset](uint32_t index) {
//                     return index + indexOffset;
//                 }
//             );
//
//             // TODO: need to file the texture id
//             auto diffuseId = material->GetTextureBindPoint(mesh->GetDiffuseTexture());
//             auto ambientId = material->GetTextureBindPoint(mesh->GetAmbientTexture());
//             for(auto& vertex : vertices) {
//                 vertex.diffuseTextureId = diffuseId;
//                 vertex.ambientTextureId = ambientId;
//             }
//
//             vertexBuffer->SetData(vertices.data(), mesh->GetVertexByte(), vertexoOffset);
//             indexBuffer->SetData(indices, indexOffset);
//             vertexoOffset += mesh->GetVertexByte();
//             indexOffset += indices.size();
//         }
//
//         vertexBuffer->SetLayout(GetMeshVertexInfoLayout());
//
//         drawBatch->SetVertexBuffer(std::move(vertexBuffer));
//         drawBatch->SetVertexArray(std::move(vertexArray));
//         drawBatch->SetIndexBuffer(std::move(indexBuffer));
//
//         m_drawBatches.push_back(std::move(drawBatch));
//         m_materials.push_back(std::move(material));
//     }
// }

std::unique_ptr<Scene> Scene::CreateSceneFromFile(const Path& sceneFile,
                                                  ResourceManager* resourceManager) {

    Assimp::Importer importer;

    auto filename = sceneFile.string();
    const auto* assimpScene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate |
                                                                  aiProcess_FlipUVs);

    if (assimpScene == nullptr) {
        auto errorStr = String(importer.GetErrorString());
        LOG(ERROR) << FORMAT("can't load scene because: {}", errorStr);
        return nullptr;
    }

    auto scene = std::make_unique<Scene>(sceneFile.parent_path(), resourceManager);
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
    scene->ProcessNode(rootNode, resourceManager, assimpScene, aRootNode);
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
