#pragma once

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
  void
  SetShader(const std::shared_ptr<ShaderResource>& shaderResource) {
    m_shaderResource = shaderResource;
  }

  std::shared_ptr<Shader>
  GetShader() {
    if (m_shaderResource != nullptr) {
      return m_shaderResource->GetShader();
    }
    return nullptr;
  }

  void
  SetAmbientTexture(const std::shared_ptr<Texture2DResource>& ambientTexture) {
    m_ambientTexture = ambientTexture;
  }

  std::shared_ptr<Texture2D>
  GetAmbientTexture() const {
    if (m_ambientTexture != nullptr) {
      return m_ambientTexture->GetTexture();
    }
    return nullptr;
  }

  void
  SetDiffuseTexture(const std::shared_ptr<Texture2DResource>& diffuseTexture) {
    m_diffuseTexture = diffuseTexture;
  }

  std::shared_ptr<Texture2D>
  GetDiffuseTexture() const {
    if (m_diffuseTexture != nullptr) {
      return m_diffuseTexture->GetTexture();
    }
    return nullptr;
  }

  void
  LoadResource(RHIFactory* rhiFactory, std::shared_ptr<ResourceManager>&) override;

 private:
  std::shared_ptr<Texture2DResource> m_diffuseTexture = nullptr;
  std::shared_ptr<Texture2DResource> m_ambientTexture = nullptr;
  std::shared_ptr<Texture2DResource> m_diffuserPBRTexture = nullptr;
  std::shared_ptr<Texture2DResource> m_ambientPBRTexture = nullptr;

  std::shared_ptr<ShaderResource> m_shaderResource = nullptr;
  // std::unique_ptr<Material> m_material;
};

class CubeMapResource final : public ResourceBase {
 public:
  explicit CubeMapResource(const CubeMapCreateInfo& createInfo)
      : ResourceBase(), m_createInfo(createInfo) {}

 public:
  void
  LoadResource(RHIFactory* rhiFactory, std::shared_ptr<ResourceManager>&) override;

  [[nodiscard]] TextureCubeMap*
  GetCubeMapTexture() const noexcept {
    if (!m_isLoad) return nullptr;
    if (m_cubeMapTexture == nullptr) return nullptr;
    return m_cubeMapTexture.get();
  }

 private:
  CubeMapCreateInfo m_createInfo;

  std::shared_ptr<TextureCubeMap> m_cubeMapTexture;
};

}  // namespace Marbas
