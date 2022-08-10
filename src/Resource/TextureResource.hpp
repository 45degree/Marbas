#pragma once

#include "RHI/RHI.hpp"
#include "Resource/ResourceBase.hpp"

namespace Marbas {

class Texture2DResource final : public ResourceBase {
 public:
  explicit Texture2DResource(const Path& path, uint32_t levels = 1)
      : ResourceBase(), m_path(path), m_levels(levels) {}

 public:
  [[nodiscard]] std::shared_ptr<Texture2D>
  GetTexture() const noexcept {
    return m_texture;
  }

  [[nodiscard]] const Path&
  GetPath() const noexcept {
    return m_path;
  }

  void
  LoadResource(RHIFactory* rhiFactory, std::shared_ptr<ResourceManager>&) override {
    if (m_isLoad) return;

    m_texture = rhiFactory->CreateTexutre2D(m_path, m_levels);

    m_isLoad = true;
  }

 private:
  Path m_path;
  uint32_t m_levels;
  std::shared_ptr<Texture2D> m_texture = nullptr;
};

class TextureCubeMapResource final : public ResourceBase {
 public:
  explicit TextureCubeMapResource(const CubeMapCreateInfo& createInfo)
      : ResourceBase(), m_createInfo(createInfo) {}

 public:
  [[nodiscard]] std::shared_ptr<TextureCubeMap>
  GetTextureCubeMap() const noexcept {
    return m_textureCubeMap;
  }

  [[nodiscard]] const CubeMapCreateInfo&
  GetCreateInfo() const noexcept {
    return m_createInfo;
  }

  void
  LoadResource(RHIFactory* rhiFactory, std::shared_ptr<ResourceManager>&) override {
    if (m_isLoad) return;

    m_textureCubeMap = rhiFactory->CreateTextureCubeMap(m_createInfo);

    m_isLoad = true;
  }

 private:
  CubeMapCreateInfo m_createInfo;
  std::shared_ptr<TextureCubeMap> m_textureCubeMap = nullptr;
};

}  // namespace Marbas
