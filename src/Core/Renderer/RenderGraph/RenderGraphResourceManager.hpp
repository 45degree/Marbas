#pragma once

#include "Common/Common.hpp"
#include "RHIFactory.hpp"
#include "RenderGraphResource.hpp"

namespace Marbas {

class RenderGraphResourceManager final {
 public:
  RenderGraphResourceManager(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {}
  ~RenderGraphResourceManager() = default;

 public:
  RenderGraphTextureHandler
  CreateTexture(const char* name, const ImageCreateInfo& createInfo) {
    m_graphTexture.push_back(details::RenderGraphTexture(name, m_rhiFactory, createInfo));
    RenderGraphTextureHandler handler;
    handler.index = m_graphTexture.size() - 1;
    return handler;
  }

  ImageView*
  GetImageView(RenderGraphTextureHandler handler, uint32_t baseLayer = 0, uint32_t layerCount = 1,
               uint32_t baseLevel = 0, uint32_t levelCount = 1) {
    return m_graphTexture[handler.index].GetImageView(baseLayer, layerCount, baseLevel, levelCount);
  }

  ImageView*
  GetImageView(std::string_view name, uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseLevel = 0,
               uint32_t levelCount = 1) {
    auto iter = std::find_if(m_graphTexture.begin(), m_graphTexture.end(),
                             [&](auto&& texture) { return texture.GetName() == name; });
    if (iter != m_graphTexture.end()) {
      return iter->GetImageView(baseLayer, layerCount, baseLevel, levelCount);
    }
    return nullptr;
  }

  RenderGraphTextureHandler
  AddExternTexture(const char* name, Image* image);

 public:
  RHIFactory* m_rhiFactory = nullptr;
  Vector<details::RenderGraphTexture> m_graphTexture;
};

}  // namespace Marbas
