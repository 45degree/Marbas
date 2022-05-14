#include "Resource/ResourceManager.hpp"

namespace Marbas {

ResourceManager::ResourceManager(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {
  auto* shaderResource = AddShader(ShaderFileInfo{.type = ShaderCodeType::FILE,
                                                  .vertexShaderPath = "shader/shader.vert.glsl",
                                                  .fragmentShaderPath = "shader/shader.frag.glsl"});

  auto* cubeMapShaderResource =
      AddShader(ShaderFileInfo{.type = ShaderCodeType::FILE,
                               .vertexShaderPath = "shader/cubeMap.vert.glsl",
                               .fragmentShaderPath = "shader/cubeMap.frag.glsl"});

  m_defaultShaderResource = shaderResource;
  m_defaultCubeMapShaderResource = cubeMapShaderResource;
}

Texture2DResource* ResourceManager::AddTexture(const Path& imagePath) {
  const auto pathString = imagePath.string();
  if (m_staticResourcePath.find(pathString) != m_staticResourcePath.end()) {
    auto uid = m_staticResourcePath.at(pathString);
    const auto& resouce = m_resources.at(uid);
    return dynamic_cast<Texture2DResource*>(resouce.get());
  }

  auto texture2DResource = std::make_unique<Texture2DResource>(imagePath);
  auto* texture2DResourcePtr = texture2DResource.get();
  auto uid = texture2DResource->GetUid();

  m_staticResourcePath[pathString] = uid;
  m_resources.insert({uid, std::move(texture2DResource)});
  m_texture2DResources.insert({uid, texture2DResourcePtr});

  return texture2DResourcePtr;
}

ShaderResource* ResourceManager::AddShader(const ShaderFileInfo& shaderFileInfo) {
  auto shaderResource = std::make_unique<ShaderResource>(shaderFileInfo);
  auto uid = shaderResource->GetUid();
  auto shaderResourcePtr = shaderResource.get();

  m_resources.insert({uid, std::move(shaderResource)});
  m_shaderResources.insert({uid, shaderResourcePtr});
  return shaderResourcePtr;
}

MaterialResource* ResourceManager::AddMaterial() {
  auto materialResource = std::make_unique<MaterialResource>();
  auto materialResourcePtr = materialResource.get();
  auto uid = materialResource->GetUid();

  materialResource->SetShader(m_defaultShaderResource);

  m_resources.insert({uid, std::move(materialResource)});
  m_materialResources.insert({uid, materialResourcePtr});

  return materialResourcePtr;
}

void ResourceManager::RemoveResource(const Uid& uid) {}

CubeMapResource* ResourceManager::AddCubeMap(const CubeMapCreateInfo& createInfo) {
  auto cubeMapResource = std::make_unique<CubeMapResource>(createInfo);

  auto cubeMapTextureResourcePtr = cubeMapResource.get();

  auto uid = cubeMapResource->GetUid();
  m_resources.insert({uid, std::move(cubeMapResource)});
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

CubeMapResource* ResourceManager::FindCubeMapResource(const Uid& uid) const noexcept {
  if (m_cubeMapMaterialResources.find(uid) == m_cubeMapMaterialResources.end()) {
    LOG(WARNING) << FORMAT("can't find cubemap material resource which's uid is is {}", uid);
    return nullptr;
  }

  return m_cubeMapMaterialResources.at(uid);
}

}  // namespace Marbas
