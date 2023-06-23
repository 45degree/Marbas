#include "RenderGraphResourceManager.hpp"

#include <glog/logging.h>

#include <algorithm>

namespace Marbas {

RenderGraphTextureHandler
RenderGraphResourceManager::CreateTexture(std::string_view name, const ImageCreateInfo& createInfo) {
  if (m_textureResLUT.find(std::string(name)) != m_textureResLUT.end()) {
    DLOG(WARNING) << "no need to create the texture resource, beacuse it's existed";
    return m_textureResLUT.at(std::string(name));
  }
  m_graphTexture.push_back(details::RenderGraphTexture(name, m_rhiFactory, createInfo));
  RenderGraphTextureHandler handler;
  handler.index = m_graphTexture.size() - 1;
  m_textureResLUT.insert({std::string(name), handler});
  return handler;
}

ImageView*
RenderGraphResourceManager::GetImageView(std::string_view name, uint32_t baseLayer, uint32_t layerCount,
                                         uint32_t baseLevel, uint32_t levelCount) {
  auto iter = m_textureResLUT.find(std::string(name));
  if (iter == m_textureResLUT.end()) {
    DLOG(WARNING) << FORMAT("can't find texture resource: {} in graph resource manager", name);
    return nullptr;
  }
  auto handler = iter->second;
  return GetImageView(handler, baseLayer, layerCount, baseLevel, levelCount);
}

}  // namespace Marbas
