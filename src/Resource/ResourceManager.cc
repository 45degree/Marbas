#include "Resource/ResourceManager.hpp"

namespace Marbas {

ResourceManager::ResourceManager()
    : m_shaderResourceContainer(std::make_shared<DefaultShaderResourceContainer>()),
      m_modelResourceContainer(std::make_shared<DefaultModelResourceContainer>()),
      m_texture2DResourceContainer(std::make_shared<DefaultTexture2DResourceContainer>()),
      m_cubeMapResourceContainer(std::make_shared<DefaultCubeMapResourceContainer>()),
      m_materialResourceContainer(std::make_shared<DefaultMaterialResourceContainer>())
  {
  auto defaultShaderResource = m_shaderResourceContainer->CreateResource(ShaderFileInfo{
      .type = ShaderCodeType::FILE,
      .vertexShaderPath = "Shader/shader.vert.glsl",
      .fragmentShaderPath = "Shader/shader.frag.glsl",
  });

  auto defaultCubeMapShaderResource = m_shaderResourceContainer->CreateResource(ShaderFileInfo{
      .type = ShaderCodeType::FILE,
      .vertexShaderPath = "Shader/cubeMap.vert.glsl",
      .fragmentShaderPath = "Shader/cubeMap.frag.glsl",
  });

  m_defaultShaderResourceUid = m_shaderResourceContainer->AddResource(defaultShaderResource);
  m_defaultCubeMapShaderResourceUid =
      m_shaderResourceContainer->AddResource(defaultCubeMapShaderResource);
}

}  // namespace Marbas
