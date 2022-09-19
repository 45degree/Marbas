#pragma once

#include "RHI/RHI.hpp"
#include "Resource/ResourceBase.hpp"

namespace Marbas {

class Texture2DResource final : public ResourceBase {
 public:
  explicit Texture2DResource(const Path& path, uint32_t levels = 1, bool isHDR = false)
      : ResourceBase(), m_path(path), m_levels(levels), m_isHDR(isHDR) {}

 public:
  [[nodiscard]] std::shared_ptr<Texture>
  GetTexture() const noexcept {
    return m_texture;
  }

  [[nodiscard]] const Path&
  GetPath() const noexcept {
    return m_path;
  }

  void
  LoadResource(RHIFactory* rhiFactory, const ResourceManager* resourceManager) override;

 private:
  Path m_path;
  uint32_t m_levels;
  bool m_isHDR = false;
  std::shared_ptr<Texture> m_texture = nullptr;
};

class TextureCubeMapResource final : public ResourceBase {
 public:
  explicit TextureCubeMapResource(const CubeMapCreateInfo& createInfo)
      : ResourceBase(), m_createInfo(createInfo) {}

 public:
  [[nodiscard]] std::shared_ptr<Texture>
  GetTextureCubeMap() const noexcept {
    return m_textureCubeMap;
  }

  [[nodiscard]] const CubeMapCreateInfo&
  GetCreateInfo() const noexcept {
    return m_createInfo;
  }

  void
  LoadResource(RHIFactory* rhiFactory, const ResourceManager*) override {
    if (m_isLoad) return;

    m_textureCubeMap = rhiFactory->CreateTextureCubeMap(m_createInfo, 1);

    m_isLoad = true;
  }

 private:
  CubeMapCreateInfo m_createInfo;
  std::shared_ptr<Texture> m_textureCubeMap = nullptr;
};

}  // namespace Marbas
