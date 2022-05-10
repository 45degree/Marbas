#include "Resource/ResourceManager.hpp"

namespace Marbas {

Texture2DResource* ResourceManager::AddTexture(const Path& imagePath) {
  const auto pathString = imagePath.string();
  if (m_staticResourcePath.find(pathString) != m_staticResourcePath.end()) {
    auto uid = m_staticResourcePath.at(pathString);
    const auto& resouce = m_resources.at(uid);
    return dynamic_cast<Texture2DResource*>(resouce.get());
  }

  auto texture = m_rhiFactory->CreateTexutre2D(imagePath);
  auto texture2DResource = std::make_unique<Texture2DResource>(std::move(texture));
  auto* texture2DResourcePtr = texture2DResource.get();
  auto uid = texture2DResource->GetUid();

  m_staticResourcePath[pathString] = uid;
  m_resources.insert({uid, std::move(texture2DResource)});
  m_texture2DResources.insert({uid, texture2DResourcePtr});

  return texture2DResourcePtr;
}

ShaderResource* ResourceManager::AddShader(const ShaderFileInfo& shaderFileInfo) {
  auto vertexShaderCode = m_rhiFactory->CreateShaderCode(
      shaderFileInfo.vertexShaderPath, shaderFileInfo.type, ShaderType::VERTEX_SHADER);

  auto fragmentShaderCode = m_rhiFactory->CreateShaderCode(
      shaderFileInfo.fragmentShaderPath, shaderFileInfo.type, ShaderType::FRAGMENT_SHADER);

  auto shader = m_rhiFactory->CreateShader();

  auto shaderResource = std::make_unique<ShaderResource>(std::move(shader));
  shaderResource->SetVertexShader(std::move(vertexShaderCode));
  shaderResource->SetFragmentShader(std::move(fragmentShaderCode));

  auto uid = shaderResource->GetUid();
  auto shaderResourcePtr = shaderResource.get();

  m_resources.insert({uid, std::move(shaderResource)});
  m_shaderResources.insert({uid, shaderResourcePtr});
  return shaderResourcePtr;
}

MaterialResource* ResourceManager::AddMaterial() {
  auto material = std::make_unique<DefaultMaterial>();
  auto materialResource = std::make_unique<MaterialResource>(std::move(material));
  auto materialResourcePtr = materialResource.get();
  auto uid = materialResource->GetUid();

  materialResource->SetShader(m_defaultShaderResource);

  m_resources.insert({uid, std::move(materialResource)});
  m_materialResources.insert({uid, materialResourcePtr});

  return materialResourcePtr;
}

void ResourceManager::RemoveResource(const Uid& uid) {}

CubeMapMaterialResource* ResourceManager::AddCubeMapMaterial(const CubeMapCreateInfo& createInfo) {
  auto cubeMapMaterial = std::make_unique<CubeMapMaterial>();
  auto cubeMapMaterialResource =
      std::make_unique<CubeMapMaterialResource>(std::move(cubeMapMaterial));
  cubeMapMaterialResource->SetShader(m_defaultCubeMapShaderResource);

  auto cubeMapTexture = m_rhiFactory->CreateTextureCubeMap(createInfo);
  cubeMapMaterialResource->SetCubeMapTexture(std::move(cubeMapTexture));

  auto cubeMapTextureResourcePtr = cubeMapMaterialResource.get();

  auto uid = cubeMapMaterialResource->GetUid();
  m_resources.insert({uid, std::move(cubeMapMaterialResource)});
  m_cubeMapMaterialResources.insert({uid, cubeMapTextureResourcePtr});

  return cubeMapTextureResourcePtr;
}

Texture2DResource* ResourceManager::FindTexture(const Uid& uid) const noexcept {
  if (m_texture2DResources.find(uid) == m_texture2DResources.end()) {
    LOG(WARNING) << FORMAT("can't find texture resource which's uid is {}", uid);
    return nullptr;
  }

  return m_texture2DResources.at(uid);
}

ShaderResource* ResourceManager::FindShaderResource(const Uid& uid) const noexcept {
  if (m_shaderResources.find(uid) == m_shaderResources.end()) {
    LOG(WARNING) << FORMAT("can't find shader resource which's uid is is {}", uid);
    return nullptr;
  }

  return m_shaderResources.at(uid);
}

MaterialResource* ResourceManager::FindMaterialResource(const Uid& uid) const noexcept {
  if (m_materialResources.find(uid) == m_materialResources.end()) {
    LOG(WARNING) << FORMAT("can't find material resource which's uid is is {}", uid);
    return nullptr;
  }

  return m_materialResources.at(uid);
}

CubeMapMaterialResource* ResourceManager::FindCubeMapMaterialResource(
    const Uid& uid) const noexcept {
  if (m_cubeMapMaterialResources.find(uid) == m_cubeMapMaterialResources.end()) {
    LOG(WARNING) << FORMAT("can't find cubemap material resource which's uid is is {}", uid);
    return nullptr;
  }

  return m_cubeMapMaterialResources.at(uid);
}

DefaultMaterial* MaterialResource::LoadMaterial(ResourceManager* resourceManager) const {
  LOG(INFO) << FORMAT("load material resource, uid is {}", m_id);

  for (const auto& diffuseTexUid : m_diffuseTextureUids) {
    auto* diffuseTexResource = resourceManager->FindTexture(diffuseTexUid);
    if (diffuseTexResource != nullptr) {
      m_material->SetDiffuseTexture(diffuseTexResource->GetTexture());
    }
  }

  for (const auto& ambientTexUid : m_ambientTextureUids) {
    auto* ambientTexResource = resourceManager->FindTexture(ambientTexUid);
    if (ambientTexResource != nullptr) {
      m_material->SetAmbientTexture(ambientTexResource->GetTexture());
    }
  }

  auto* shaderResource = resourceManager->FindShaderResource(m_shaderResource);
  LOG_IF(WARNING, shaderResource == nullptr) << "can't set shader for material";
  if (shaderResource != nullptr) {
    m_material->SetShader(shaderResource->LoadShader());
  }

  m_isLoad = true;

  return m_material.get();
}

CubeMapMaterial* CubeMapMaterialResource::LoadResource(ResourceManager* resourceManager) const {
  LOG(INFO) << FORMAT("load cubemap material, uid is {}", m_id);

  m_material->SetCubeMapTexture(m_cubeMapTexture.get());

  auto* shaderResource = resourceManager->FindShaderResource(m_shaderResource);
  LOG_IF(WARNING, shaderResource == nullptr) << "can't set shader for material";
  if (shaderResource != nullptr) {
    m_material->SetShader(shaderResource->LoadShader());
  }

  m_isLoad = true;
  return m_material.get();
}

}  // namespace Marbas
