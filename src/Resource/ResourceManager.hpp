#ifndef MARBAS_RESOURCE_RESOURCE_MANAGER_HPP
#define MARBAS_RESOURCE_RESOURCE_MANAGER_HPP

#include <concepts>

#include "Common.hpp"
#include "Resource/MaterialResource.hpp"
#include "Resource/ResourceBase.hpp"
#include "Resource/ShaderResource.hpp"
#include "Resource/TextureResource.hpp"

namespace Marbas {

class ResourceManager {
 public:
  explicit ResourceManager(RHIFactory* rhiFactory);

  ~ResourceManager() = default;

 public:
  Texture2DResource* AddTexture(const Path& imagePath);

  ShaderResource* AddShader(const ShaderFileInfo& shaderFileInfo);

  MaterialResource* AddMaterial();

  CubeMapResource* AddCubeMap(const CubeMapCreateInfo& createInfo);

  void RemoveResource(const Uid& id);

  [[nodiscard]] ShaderResource* GetDefaultCubeMapShader() const noexcept {
    return m_defaultCubeMapShaderResource;
  }

  [[nodiscard]] ShaderResource* GetDefaultShader() const noexcept {
    return m_defaultShaderResource;
  }

  [[nodiscard]] Texture2DResource* FindTexture(const Uid& uid) const noexcept;

  [[nodiscard]] ShaderResource* FindShaderResource(const Uid& uid) const noexcept;

  [[nodiscard]] MaterialResource* FindMaterialResource(const Uid& uid) const noexcept;

  [[nodiscard]] CubeMapResource* FindCubeMapResource(const Uid& uid) const noexcept;

 private:
  RHIFactory* m_rhiFactory;

  std::unordered_map<Uid, std::unique_ptr<ResourceBase>> m_resources;
  std::unordered_map<Uid, Texture2DResource*> m_texture2DResources;
  std::unordered_map<Uid, CubeMapResource*> m_cubeMapMaterialResources;
  std::unordered_map<Uid, MaterialResource*> m_materialResources;
  std::unordered_map<Uid, ShaderResource*> m_shaderResources;

  std::unordered_map<String, Uid> m_staticResourcePath;

  ShaderResource* m_defaultShaderResource;
  ShaderResource* m_defaultCubeMapShaderResource;
};

}  // namespace Marbas

#endif
