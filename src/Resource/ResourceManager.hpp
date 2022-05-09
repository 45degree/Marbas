#ifndef MARBAS_RESOURCE_RESOURCE_MANAGER_HPP
#define MARBAS_RESOURCE_RESOURCE_MANAGER_HPP

#include <concepts>

#include "Common.hpp"
#include "Resource/MaterialResource.hpp"
#include "Resource/ResourceBase.hpp"
#include "Resource/ShaderResource.hpp"
#include "Resource/TextureResource.hpp"

namespace Marbas {

struct ShaderFileInfo {
  ShaderCodeType type;
  Path vertexShaderPath;
  Path fragmentShaderPath;
};

class ResourceManager {
 public:
  explicit ResourceManager(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {
    auto* shaderResource =
        AddShader({ShaderCodeType::FILE, "shader/shader.vert.glsl", "shader/shader.frag.glsl"});
    auto* cubeMapShaderResource =
        AddShader({ShaderCodeType::FILE, "shader/cubeMap.vert.glsl", "shader/cubeMap.frag.glsl"});

    m_defaultShaderResource = shaderResource->GetUid();
    m_defaultCubeMapShaderResource = cubeMapShaderResource->GetUid();
  }

  ~ResourceManager() = default;

 public:
  Texture2DResource* AddTexture(const Path& imagePath);

  ShaderResource* AddShader(const ShaderFileInfo& shaderFileInfo);

  MaterialResource* AddMaterial();

  CubeMapMaterialResource* AddCubeMapMaterialResource(const CubeMapCreateInfo& createInfo);

  void RemoveResource(const Uid& id);

  [[nodiscard]] Texture2DResource* FindTexture(const Uid& uid) const noexcept;

  [[nodiscard]] ShaderResource* FindShaderResource(const Uid& uid) const noexcept;

  [[nodiscard]] MaterialResource* FindMaterialResource(const Uid& uid) const noexcept;

  [[nodiscard]] CubeMapMaterialResource* FindCubeMapMaterialResource(const Uid& uid) const noexcept;

 private:
  RHIFactory* m_rhiFactory;

  std::unordered_map<Uid, std::unique_ptr<ResourceBase>> m_resources;
  std::unordered_map<Uid, Texture2DResource*> m_texture2DResources;
  std::unordered_map<Uid, CubeMapMaterialResource*> m_cubeMapMaterialResources;
  std::unordered_map<Uid, MaterialResource*> m_materialResources;
  std::unordered_map<Uid, ShaderResource*> m_shaderResources;

  std::unordered_map<String, Uid> m_staticResourcePath;

  Uid m_defaultShaderResource;
  Uid m_defaultCubeMapShaderResource;
};

}  // namespace Marbas

#endif
