#ifndef MARBAS_RESOURCE_TEXTURE_RESOURCE_HPP
#define MARBAS_RESOURCE_TEXTURE_RESOURCE_HPP

#include "RHI/RHI.hpp"
#include "Resource/ResourceBase.hpp"

namespace Marbas {

class Texture2DResource final : public ResourceBase {
 public:
  explicit Texture2DResource(std::unique_ptr<Texture2D>&& texture)
      : ResourceBase(), m_texture(std::move(texture)) {}

 public:
  [[nodiscard]] Texture2D* GetTexture() const noexcept {
    if (m_texture == nullptr) return nullptr;
    return m_texture.get();
  }

  [[nodiscard]] const Path& GetPath() const noexcept { return m_path; }

  void SetPath(const Path& path) { m_path = path; }

 private:
  Path m_path;
  std::unique_ptr<Texture2D> m_texture;
};

class TextureCubeMapResource final : public ResourceBase {
 public:
  explicit TextureCubeMapResource(std::unique_ptr<TextureCubeMap>&& cubeMap)
      : ResourceBase(), m_cubeMap(std::move(cubeMap)) {}
  TextureCubeMap* GetCubeMap() const noexcept { return m_cubeMap.get(); }

 private:
  std::unique_ptr<TextureCubeMap> m_cubeMap;
};

}  // namespace Marbas

#endif
