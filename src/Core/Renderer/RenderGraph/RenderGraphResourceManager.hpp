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

  RenderGraphTextureHandler
  AddExternTexture(const char* name, Image* image);

 public:
  RHIFactory* m_rhiFactory = nullptr;
  Vector<details::RenderGraphTexture> m_graphTexture;
};

}  // namespace Marbas
