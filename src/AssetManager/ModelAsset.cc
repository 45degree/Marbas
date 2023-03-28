#include "ModelAsset.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <assimp/Importer.hpp>

#include "AssetManager/GPUAssetUpLoader.hpp"
#include "AssetManager/TextureAsset.hpp"

namespace Marbas {

static std::optional<String>
LoadTexturesFromMaterial(const aiMaterial* aMaterial, const aiTextureType type, const Path& current) {
  auto projectDir = AssetRegistry::GetInstance()->GetProjectDir();

  // Vector<std::shared_ptr<Texture2DResource>> texturesId;
  for (auto i = 0; i < aMaterial->GetTextureCount(type); i++) {
    aiString str;
    aMaterial->GetTexture(type, i, &str);
    auto textureRelativePath = std::filesystem::relative(current / Path(str.C_Str()), projectDir);
    auto textureResPath = "res://" + textureRelativePath.string();
    return textureResPath;
  }

  return std::nullopt;
}

static Mesh
ReadMeshFromNode(const aiMesh* aMesh, const aiScene* aScene, const Path& current) {
  Mesh mesh;
  mesh.m_name = aMesh->mName.C_Str();

  // set vertex buffer
  auto* texture = aMesh->mTextureCoords[0];
  for (unsigned int i = 0; i < aMesh->mNumVertices; i++) {
    auto vertex = aMesh->mVertices[i];
    auto normal = aMesh->mNormals[i];
    auto tangents = aMesh->mTangents[i];
    auto bitangents = aMesh->mBitangents[i];

    Vertex vertexInfo{
        .posX = vertex.x,
        .posY = vertex.y,
        .posZ = vertex.z,
        .normalX = normal.x,
        .normalY = normal.y,
        .normalZ = normal.z,
        .tangentX = tangents.x,
        .tangentY = tangents.y,
        .tangentZ = tangents.z,
        .bitangentX = bitangents.x,
        .bitangentY = bitangents.y,
        .bitangentZ = bitangents.z,
    };

    if (texture == nullptr) {
      vertexInfo.textureU = 0;
      vertexInfo.textureV = 0;
    } else {
      vertexInfo.textureU = texture[i].x;
      vertexInfo.textureV = texture[i].y;
    }

    mesh.m_vertices.push_back(vertexInfo);
  }

  // set face (index buffer)
  for (unsigned int i = 0; i < aMesh->mNumFaces; i++) {
    auto face = aMesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      mesh.m_indices.push_back(face.mIndices[j]);
    }
  }

  // set material
  if (aMesh->mMaterialIndex >= 0) {
    auto* material = aScene->mMaterials[aMesh->mMaterialIndex];
    auto diffuseTexture = LoadTexturesFromMaterial(material, aiTextureType_DIFFUSE, current);
    auto ambientTexture = LoadTexturesFromMaterial(material, aiTextureType_AMBIENT_OCCLUSION, current);
    auto normalTexture = LoadTexturesFromMaterial(material, aiTextureType_NORMALS, current);
    auto roughnessTexture = LoadTexturesFromMaterial(material, aiTextureType_DIFFUSE_ROUGHNESS, current);
    auto metalnessTexture = LoadTexturesFromMaterial(material, aiTextureType_METALNESS, current);

    mesh.m_material.m_diffuseTexturePath = diffuseTexture;
    mesh.m_material.m_useDiffuseTexture = mesh.m_material.m_diffuseTexturePath.has_value();

    mesh.m_material.m_ambientTexturePath = ambientTexture;
    mesh.m_material.m_useAmbientTexture = mesh.m_material.m_ambientTexturePath.has_value();

    mesh.m_material.m_normalTexturePath = normalTexture;
    mesh.m_material.m_useNormalTexture = mesh.m_material.m_normalTexturePath.has_value();

    mesh.m_material.m_roughnessTexturePath = roughnessTexture;
    mesh.m_material.m_useRoughnessTexture = mesh.m_material.m_roughnessTexturePath.has_value();

    mesh.m_material.m_metalnessTexturePath = metalnessTexture;
    mesh.m_material.m_useMetalnessTexture = mesh.m_material.m_metalnessTexturePath.has_value();
  }

  return mesh;
}

static void
ProcessSubNode(const aiScene* aScene, const aiNode* aNode, const Path& currentDir, Vector<Mesh>& meshes) {
  if (aNode->mNumMeshes > 0) {
    for (int i = 0; i < aNode->mNumMeshes; i++) {
      auto aMesh = aScene->mMeshes[aNode->mMeshes[i]];
      meshes.push_back(ReadMeshFromNode(aMesh, aScene, currentDir));
    }
  }

  for (int i = 0; i < aNode->mNumChildren; i++) {
    ProcessSubNode(aScene, aNode->mChildren[i], currentDir, meshes);
  }
}

std::shared_ptr<ModelAsset>
ModelAsset::Load(const Path& path) {
  Assimp::Importer importer;
  auto filename = path.filename().string();

  const auto pos = filename.find_last_of('.');
  const auto modelName = filename.substr(0, pos);

  int assimpFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
  const auto* assimpScene = importer.ReadFile(path.string(), assimpFlags);

  if (assimpScene == nullptr) {
    auto errorStr = String(importer.GetErrorString());
    LOG(ERROR) << FORMAT("can't load assimp scene because: {}", errorStr);
  }

  auto modelAsset = std::make_shared<ModelAsset>();
  auto& model = modelAsset->m_model;

  auto aRootNode = assimpScene->mRootNode;
  ProcessSubNode(assimpScene, aRootNode, path.parent_path(), model);

  modelAsset->SetModelName(modelName);
  for (auto&& mesh : model) {
    // auto volume = std::make_unique<AABB>(AABB::GenerateAABB(mesh));
    // auto center = glm::to_string(volume->GetCenter());
    // auto extent = glm::to_string(volume->GetExtent());
    // DLOG(INFO) << FORMAT("generate a AABB vloume, center:{},  extent:{}", center, extent);
    // mesh.m_volume = std::move(volume);
  }
  return modelAsset;
}

/**
 * Model GPU Asset
 */

ModelGPUAsset::MeshGPUAsset::MeshGPUAsset(const Mesh& mesh, RHIFactory* rhiFactory)
    : m_rhiFactory(rhiFactory), m_indexCount(mesh.m_indices.size()) {
  auto bufCtx = rhiFactory->GetBufferContext();
  auto vertexBufferSize = sizeof(Vertex) * mesh.m_vertices.size();
  auto vertexBufferData = mesh.m_vertices.data();
  auto indexBufferSize = sizeof(uint32_t) * mesh.m_indices.size();
  auto indexBufferData = mesh.m_indices.data();
  m_vertexBuffer = bufCtx->CreateBuffer(BufferType::VERTEX_BUFFER, vertexBufferData, vertexBufferSize, false);
  m_indexBuffer = bufCtx->CreateBuffer(BufferType::INDEX_BUFFER, indexBufferData, indexBufferSize, false);

  m_materialInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_materialInfo, sizeof(MaterialInfo), false);
  m_transformBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, nullptr, sizeof(glm::mat4), false);

  Update(mesh);
}

ModelGPUAsset::MeshGPUAsset::~MeshGPUAsset() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->DestroyBuffer(m_vertexBuffer);
  bufCtx->DestroyBuffer(m_indexBuffer);
}

void
ModelGPUAsset::MeshGPUAsset::Update(const Mesh& mesh) {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto textureManager = AssetManager<TextureAsset>::GetInstance();
  auto textureGPUManager = GPUAssetManager<TextureGPUAsset>::GetInstance();

  auto SetTexture = [&](const AssetPath& path) {
    if (!textureManager->Existed(path)) {
      textureManager->Create(path);
    }
    auto asset = textureManager->Get(path);
    if (!textureGPUManager->Exists(asset->GetUid())) {
      textureGPUManager->Create(asset, m_rhiFactory);
    }

    // remove the asset
    textureManager->RemoveFromCache(asset->GetUid());

    return textureGPUManager->Get(asset->GetUid());
  };

  if (mesh.m_material.m_ambientTexturePath.has_value()) {
    m_aoTexture = SetTexture(*mesh.m_material.m_ambientTexturePath);
  }
  m_materialInfo.hasAoTex = mesh.m_material.m_useAmbientTexture;

  if (mesh.m_material.m_diffuseTexturePath.has_value()) {
    m_diffuseTexture = SetTexture(*mesh.m_material.m_diffuseTexturePath);
  }
  m_materialInfo.hasDiffuseTex = mesh.m_material.m_useDiffuseTexture;
  const auto& color = mesh.m_material.m_diffuseColor;
  m_materialInfo.diffuseColor = glm::vec4(color[0], color[1], color[2], color[3]);

  if (mesh.m_material.m_metalnessTexturePath.has_value()) {
    m_metallicTexture = SetTexture(*mesh.m_material.m_metalnessTexturePath);
  }
  m_materialInfo.hasMetallicTex = mesh.m_material.m_useMetalnessTexture;
  m_materialInfo.metallicValue = mesh.m_material.m_metalnessValue;

  if (mesh.m_material.m_normalTexturePath.has_value()) {
    m_normalTexture = SetTexture(*mesh.m_material.m_normalTexturePath);
  }
  m_materialInfo.hasNormalTex = mesh.m_material.m_useNormalTexture;

  if (mesh.m_material.m_roughnessTexturePath.has_value()) {
    m_roughnessTexture = SetTexture(*mesh.m_material.m_roughnessTexturePath);
  }
  m_materialInfo.hasRoughnessTex = mesh.m_material.m_useRoughnessTexture;
  m_materialInfo.roughnessValue = mesh.m_material.m_roughnessValue;

  bufCtx->UpdateBuffer(m_materialInfoBuffer, &m_materialInfo, sizeof(MaterialInfo), false);
}

ModelGPUAsset::ModelGPUAsset(const std::shared_ptr<ModelAsset>& modelAsset, RHIFactory* rhiFactory) {
  auto& model = modelAsset->m_model;
  for (auto& mesh : model) {
    auto meshGPUAsset = std::make_shared<MeshGPUAsset>(mesh, rhiFactory);
    m_meshGPUAsset.push_back(meshGPUAsset);
  }
}

void
ModelGPUAsset::Update(const std::shared_ptr<ModelAsset>& modelAsset) {
  const auto& meshes = modelAsset->m_model;
  for (int i = 0; i < meshes.size(); i++) {
    auto gpuAsset = m_meshGPUAsset[i];
    gpuAsset->Update(meshes[i]);
  }
}

}  // namespace Marbas
