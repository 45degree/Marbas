#include "ModelAsset.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>

#include <assimp/Importer.hpp>

namespace Marbas {

static std::optional<String>
LoadTexturesFromMaterial(const aiMaterial* aMaterial, const aiTextureType type, const Path& current) {
  auto projectDir = AssetRegistry::GetInstance()->GetProjectDir();

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
    auto normalTexture = LoadTexturesFromMaterial(material, aiTextureType_NORMALS, current);
    auto roughnessTexture = LoadTexturesFromMaterial(material, aiTextureType_DIFFUSE_ROUGHNESS, current);
    auto metalnessTexture = LoadTexturesFromMaterial(material, aiTextureType_METALNESS, current);

    mesh.m_material.m_diffuseTexturePath = diffuseTexture;
    mesh.m_material.m_useDiffuseTexture = mesh.m_material.m_diffuseTexturePath.has_value();

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
  return modelAsset;
}

}  // namespace Marbas
