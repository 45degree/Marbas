#pragma once

#include "RHI/RHI.hpp"
#include "Resource/ResourceBase.hpp"

namespace Marbas {

class Texture2DResource final : public ResourceBase {
 public:
  explicit Texture2DResource(const Path& path) : ResourceBase(), m_path(path) {}

 public:
  [[nodiscard]] std::shared_ptr<Texture2D>
  GetTexture() const noexcept {
    if (m_texture == nullptr) return nullptr;
    return m_texture;
  }

  [[nodiscard]] const Path&
  GetPath() const noexcept {
    return m_path;
  }

  void
  LoadResource(RHIFactory* rhiFactory, std::shared_ptr<ResourceManager>&) override {
    if (m_isLoad) return;

    m_texture = rhiFactory->CreateTexutre2D(m_path);

    m_isLoad = true;
  }

 private:
  Path m_path;
  std::shared_ptr<Texture2D> m_texture = nullptr;
};

}  // namespace Marbas
