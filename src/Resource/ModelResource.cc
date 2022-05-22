#include "Resource/ModelResource.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <assimp/Importer.hpp>

#include "Resource/ResourceManager.hpp"

namespace Marbas {

static Vector<std::shared_ptr<Texture2DResource>>
LoadTexturesFromMaterial(
    const aiMaterial* aMaterial, const aiTextureType type, const Path& current,
    std::shared_ptr<IResourceContainer<Texture2DResource>>& textureResourceContainer) {
  Vector<std::shared_ptr<Texture2DResource>> texturesId;
  for (auto i = 0; i < aMaterial->GetTextureCount(type); i++) {
    aiString str;
    aMaterial->GetTexture(type, i, &str);
    Path texturePath = current / Path(str.C_Str());
    auto resource = textureResourceContainer->CreateResource(texturePath);
    textureResourceContainer->AddResource(resource);
    texturesId.push_back(resource);
  }

  return texturesId;
}

static std::shared_ptr<Mesh>
ReadMeshFromNode(const aiMesh* aMesh, const aiScene* aScene, const Path& current,
                 std::shared_ptr<ResourceManager>& resourceManager) {
  auto mesh = std::make_shared<Mesh>();
  mesh->m_name = String(aMesh->mName.C_Str());

  // set vertex buffer
  auto* texture = aMesh->mTextureCoords[0];
  for (unsigned int i = 0; i < aMesh->mNumVertices; i++) {
    auto vertex = aMesh->mVertices[i];
    auto normal = aMesh->mNormals[i];

    Vertex vertexInfo{
        .posX = vertex.x,
        .posY = vertex.y,
        .posZ = vertex.z,
        .normalX = normal.x,
        .normalY = normal.y,
        .normalZ = normal.z,
    };

    if (texture == nullptr) {
      vertexInfo.textureU = 0;
      vertexInfo.textureV = 0;
    } else {
      vertexInfo.textureU = texture[i].x;
      vertexInfo.textureV = texture[i].y;
    }

    mesh->m_vertices.push_back(vertexInfo);
  }

  // set face (index buffer)
  for (unsigned int i = 0; i < aMesh->mNumFaces; i++) {
    auto face = aMesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      mesh->m_indices.push_back(face.mIndices[j]);
    }
  }

  // set material
  if (aMesh->mMaterialIndex >= 0) {
    auto* material = aScene->mMaterials[aMesh->mMaterialIndex];
    auto textureResourceContainer = resourceManager->GetTexture2DResourceContainer();
    auto materialResourceContainer = resourceManager->GetMaterialResourceContainer();
    auto diffuseTexture = LoadTexturesFromMaterial(material, aiTextureType_DIFFUSE, current,
                                                   textureResourceContainer);
    auto ambientTexture = LoadTexturesFromMaterial(material, aiTextureType_AMBIENT, current,
                                                   textureResourceContainer);
    auto diffusePBRTexture = LoadTexturesFromMaterial(material, aiTextureType_DIFFUSE_ROUGHNESS,
                                                      current, textureResourceContainer);

    auto ambientPBRTexture = LoadTexturesFromMaterial(material, aiTextureType_AMBIENT_OCCLUSION,
                                                      current, textureResourceContainer);

    auto materialResource = materialResourceContainer->CreateResource();

    if (diffuseTexture.size() > 0) {
      materialResource->SetDiffuseTexture(diffuseTexture[0]);
    }
    if (ambientTexture.size() > 0) {
      materialResource->SetAmbientTexture(ambientTexture[0]);
    }

    auto id = materialResourceContainer->AddResource(materialResource);
    mesh->m_materialId = id;
  }

  return mesh;
}

static void
ProcessSubNode(const aiScene* aScene, const aiNode* aNode, const Path& currentDir,
               Vector<std::shared_ptr<Mesh>>& meshes,
               std::shared_ptr<ResourceManager>& resourceManager) {
  if (aNode->mNumMeshes > 0) {
    for (int i = 0; i < aNode->mNumMeshes; i++) {
      auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];
      meshes.push_back(ReadMeshFromNode(aMesh, aScene, currentDir, resourceManager));
    }
  }

  for (int i = 0; i < aNode->mNumChildren; i++) {
    ProcessSubNode(aScene, aNode->mChildren[i], currentDir, meshes, resourceManager);
  }
}

void
ModelResource::LoadResource(RHIFactory* rhiFactory,
                            std::shared_ptr<ResourceManager>& resourceManager) {
  if (m_isLoad) return;

  Assimp::Importer importer;
  auto filename = m_modelPath.filename().string();

  const auto pos = filename.find_last_of('.');
  const auto modelName = filename.substr(0, pos);

  const auto* assimpScene =
      importer.ReadFile(m_modelPath.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

  if (assimpScene == nullptr) {
    auto errorStr = String(importer.GetErrorString());
    LOG(ERROR) << FORMAT("can't load assimp scene because: {}", errorStr);
  }

  m_model = std::make_shared<Model>();
  Vector<std::shared_ptr<Mesh>> meshes;

  auto aRootNode = assimpScene->mRootNode;
  ProcessSubNode(assimpScene, aRootNode, m_modelPath.parent_path(), meshes, resourceManager);

  m_model->SetModelName(modelName);
  for (auto&& mesh : meshes) {
    m_model->AddMesh(mesh);
  }

  m_isLoad = true;
}

}  // namespace Marbas
