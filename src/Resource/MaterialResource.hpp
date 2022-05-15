#ifndef MARBAS_RESOURCE_MATERIAL_RESOURCE_HPP
#define MARBAS_RESOURCE_MATERIAL_RESOURCE_HPP

#include <glog/logging.h>

#include <set>

#include "Resource/ResourceBase.hpp"
#include "Resource/ShaderResource.hpp"
#include "Resource/TextureResource.hpp"

namespace Marbas {

class MaterialResource final : public ResourceBase {
 public:
  explicit MaterialResource() : ResourceBase() {}

 public:
  void SetShader(const std::shared_ptr<ShaderResource>& shaderResource) {
    m_shaderResource = shaderResource;
  }

  void SetAmbientTexture(const std::shared_ptr<Texture2DResource>& ambientTexture) {
    m_ambientTexture = ambientTexture;
  }

  void SetDiffuseTexture(const std::shared_ptr<Texture2DResource>& diffuseTexture) {
    m_diffuseTexture = diffuseTexture;
  }

  [[nodiscard]] Material* GetMaterial() const noexcept {
    if (!m_isLoad) return nullptr;

    return m_material.get();
  }

  void LoadResource(RHIFactory* rhiFactory) override;

 private:
  std::shared_ptr<Texture2DResource> m_diffuseTexture = nullptr;
  std::shared_ptr<Texture2DResource> m_ambientTexture = nullptr;

  std::shared_ptr<ShaderResource> m_shaderResource = nullptr;
  std::unique_ptr<Material> m_material;
};

class CubeMapResource final : public ResourceBase {
 public:
  explicit CubeMapResource(const CubeMapCreateInfo& createInfo)
      : ResourceBase(), m_createInfo(createInfo) {}

 public:
  void LoadResource(RHIFactory* rhiFactory) override;

  [[nodiscard]] TextureCubeMap* GetCubeMapTexture() const noexcept {
    if (!m_isLoad) return nullptr;
    if (m_cubeMapTexture == nullptr) return nullptr;
    return m_cubeMapTexture.get();
  }

 private:
  CubeMapCreateInfo m_createInfo;

  std::unique_ptr<TextureCubeMap> m_cubeMapTexture;
};

}  // namespace Marbas

#endif
