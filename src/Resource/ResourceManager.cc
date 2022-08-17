#include "Resource/ResourceManager.hpp"

namespace Marbas {

ResourceManager::ResourceManager()
    : m_shaderResourceContainer(std::make_shared<DefaultShaderResourceContainer>()),
      m_modelResourceContainer(std::make_shared<DefaultModelResourceContainer>()),
      m_texture2DResourceContainer(std::make_shared<DefaultTexture2DResourceContainer>()),
      m_textureCubeMapResourceContainer(std::make_shared<DefaultTextureCubeMapResourceContainer>()),
      m_materialResourceContainer(std::make_shared<DefaultMaterialResourceContainer>()) {
  auto defaultShaderResource = m_shaderResourceContainer->CreateResource();
  defaultShaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/shader.vert.glsl");
  defaultShaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/shader.frag.glsl");

  auto defaultCubeMapShaderResource = m_shaderResourceContainer->CreateResource();
  defaultCubeMapShaderResource->SetShaderStage(ShaderType::VERTEX_SHADER,
                                               "Shader/cubeMap.vert.glsl");
  defaultCubeMapShaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER,
                                               "Shader/cubeMap.frag.glsl");

  m_defaultShaderResourceUid = m_shaderResourceContainer->AddResource(defaultShaderResource);
  m_defaultCubeMapShaderResourceUid =
      m_shaderResourceContainer->AddResource(defaultCubeMapShaderResource);
}

}  // namespace Marbas
