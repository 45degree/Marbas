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
  SetAmbientOcclusionTexture(const std::shared_ptr<Texture2DResource>& aoTexture) {
    m_AOTexture = aoTexture;
  }

  std::shared_ptr<Texture>
  GetAmbientOcclusionTexture() const {
    if (m_AOTexture != nullptr) {
      return m_AOTexture->GetTexture();
    }
    return nullptr;
  }

  bool
  HasAmbientOcclusionTexture() const {
    if (m_AOTexture != nullptr) {
      return m_AOTexture->GetTexture() != nullptr;
    }
    return false;
  }

  void
  SetAlbedoTexture(const std::shared_ptr<Texture2DResource>& diffuseTexture) {
    m_albedoTexture = diffuseTexture;
  }

  void
  SetAlbedoColor(const std::array<float, 4>& color) {
    m_albedoColor = color;
  }

  std::shared_ptr<Texture>
  GetAlbedoTexture() const {
    if (m_albedoTexture != nullptr) {
      return m_albedoTexture->GetTexture();
    }
    return nullptr;
  }

  bool
  HasAlbedoTexture() const {
    if (m_albedoTexture != nullptr) {
      return m_albedoTexture->GetTexture() != nullptr;
    }
    return false;
  }

  void
  SetNormalTexture(const std::shared_ptr<Texture2DResource>& normalTexture) {
    m_normalTexture = normalTexture;
  }

  std::shared_ptr<Texture>
  GetNormalTexture() {
    if (m_normalTexture != nullptr) {
      return m_normalTexture->GetTexture();
    }
    return nullptr;
  }

  bool
  HasNormalTexture() const {
    if (m_normalTexture != nullptr) {
      return m_normalTexture->GetTexture() != nullptr;
    }
    return false;
  }

  void
  SetRoughnessTexture(const std::shared_ptr<Texture2DResource>& roughnessTexture) {
    m_roughnessTexture = roughnessTexture;
  }

  std::shared_ptr<Texture>
  GetRoughnessTexture() {
    if (m_roughnessTexture != nullptr) {
      return m_roughnessTexture->GetTexture();
    }
    return nullptr;
  }

  bool
  HasRoughnessTexture() const {
    if (m_roughnessTexture != nullptr) {
      return m_roughnessTexture->GetTexture() != nullptr;
    }
    return false;
  }

  void
  SetMetallicTexture(const std::shared_ptr<Texture2DResource>& metallicTexture) {
    m_metallicTexture = metallicTexture;
  }

  std::shared_ptr<Texture>
  GetMetallicTexture() {
    if (m_metallicTexture != nullptr) {
      return m_metallicTexture->GetTexture();
    }
    return nullptr;
  }

  bool
  HasMetallicTexture() const {
    if (m_metallicTexture != nullptr) {
      return m_metallicTexture->GetTexture() != nullptr;
    }
    return false;
  }

  void
  LoadResource(RHIFactory* rhiFactory, const ResourceManager*) override;

 private:
  // albedo
  std::shared_ptr<Texture2DResource> m_albedoTexture = nullptr;
  std::optional<std::array<float, 4>> m_albedoColor;

  // roughness
  std::shared_ptr<Texture2DResource> m_roughnessTexture = nullptr;
  std::optional<std::array<float, 4>> m_roughnessColor;

  // ambient Occlusion
  std::shared_ptr<Texture2DResource> m_AOTexture = nullptr;

  // metallic
  std::shared_ptr<Texture2DResource> m_metallicTexture = nullptr;

  // normal
  std::shared_ptr<Texture2DResource> m_normalTexture = nullptr;

  std::shared_ptr<ShaderResource> m_shaderResource = nullptr;
};

}  // namespace Marbas
