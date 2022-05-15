#include "Resource/ResourceManager.hpp"

namespace Marbas {

ResourceManager::ResourceManager() {
  auto shaderResource = AddShader(ShaderFileInfo{.type = ShaderCodeType::FILE,
                                                 .vertexShaderPath = "shader/shader.vert.glsl",
                                                 .fragmentShaderPath = "shader/shader.frag.glsl"});

  auto cubeMapShaderResource =
      AddShader(ShaderFileInfo{.type = ShaderCodeType::FILE,
                               .vertexShaderPath = "shader/cubeMap.vert.glsl",
                               .fragmentShaderPath = "shader/cubeMap.frag.glsl"});

  m_defaultShaderResource = shaderResource;
  m_defaultCubeMapShaderResource = cubeMapShaderResource;
}

std::shared_ptr<Texture2DResource> ResourceManager::AddTexture(const Path& imagePath) {
  const auto pathString = imagePath.string();
  if (m_staticResourcePath.find(pathString) != m_staticResourcePath.end()) {
    auto uid = m_staticResourcePath.at(pathString);
    return FindResource<Texture2DResource>(uid);
  }

  auto texture2DResource = std::make_shared<Texture2DResource>(imagePath);
  auto uid = texture2DResource->GetUid();
  m_staticResourcePath[pathString] = uid;

  m_resources.insert({uid, texture2DResource});

  return texture2DResource;
}

std::shared_ptr<ShaderResource> ResourceManager::AddShader(const ShaderFileInfo& shaderFileInfo) {
  auto shaderResource = std::make_shared<ShaderResource>(shaderFileInfo);
  auto uid = shaderResource->GetUid();

  m_resources.insert({uid, shaderResource});
  return shaderResource;
}

std::shared_ptr<MaterialResource> ResourceManager::AddMaterial() {
  auto materialResource = std::make_shared<MaterialResource>();
  auto uid = materialResource->GetUid();

  materialResource->SetShader(m_defaultShaderResource);

  m_resources.insert({uid, materialResource});

  return materialResource;
}

void ResourceManager::RemoveResource(const Uid& uid) {}

std::shared_ptr<CubeMapResource> ResourceManager::AddCubeMap(const CubeMapCreateInfo& createInfo) {
  auto cubeMapResource = std::make_shared<CubeMapResource>(createInfo);

  auto uid = cubeMapResource->GetUid();
  m_resources.insert({uid, cubeMapResource});

  return cubeMapResource;
}

}  // namespace Marbas
