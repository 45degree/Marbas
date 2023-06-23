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
  CreateTexture(std::string_view name, const ImageCreateInfo& createInfo);

  ImageView*
  GetImageView(RenderGraphTextureHandler handler, uint32_t baseLayer = 0, uint32_t layerCount = 1,
               uint32_t baseLevel = 0, uint32_t levelCount = 1) {
    return m_graphTexture[handler.index].GetImageView(baseLayer, layerCount, baseLevel, levelCount);
  }

  ImageView*
  GetImageView(std::string_view name, uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseLevel = 0,
               uint32_t levelCount = 1);

  RenderGraphTextureHandler
  GetHandler(std::string_view name) {
    return m_textureResLUT.at(std::string(name));
  }

  RenderGraphTextureHandler
  AddExternalTexture(std::string_view name, Image* image);

 public:
  RHIFactory* m_rhiFactory = nullptr;
  Vector<details::RenderGraphTexture> m_graphTexture;

  std::unordered_map<std::string, RenderGraphTextureHandler> m_textureResLUT;
};

}  // namespace Marbas
