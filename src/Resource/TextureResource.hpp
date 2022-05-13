#ifndef MARBAS_RESOURCE_TEXTURE_RESOURCE_HPP
#define MARBAS_RESOURCE_TEXTURE_RESOURCE_HPP

#include "RHI/RHI.hpp"
#include "Resource/ResourceBase.hpp"

namespace Marbas {

class Texture2DResource final : public ResourceBase {
 public:
  explicit Texture2DResource(const Path& path) : ResourceBase(), m_path(path) {}

 public:
  [[nodiscard]] Texture2D* GetTexture() const noexcept {
    if (m_texture == nullptr) return nullptr;
    return m_texture.get();
  }

  [[nodiscard]] const Path& GetPath() const noexcept { return m_path; }

  void LoadResource(RHIFactory* rhiFactory) override {
    if (m_isLoad) return;

    m_texture = rhiFactory->CreateTexutre2D(m_path);

    m_isLoad = true;
  }

 private:
  Path m_path;
  std::unique_ptr<Texture2D> m_texture;
};

}  // namespace Marbas

#endif
