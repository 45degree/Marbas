#pragma once

#include <stdint.h>

#include "RHIFactory.hpp"
#include "RenderGraphNode.hpp"

namespace Marbas {

namespace details {

class RenderGraphResource : public RenderGraphNode {
 public:
  RenderGraphResource(const char* name, RHIFactory* rhiFactory)
      : RenderGraphNode(name, RenderGraphNodeType::Resource), m_rhiFactory(rhiFactory) {}

  virtual void
  Create() = 0;

  virtual bool
  IsCreate() = 0;

 protected:
  RHIFactory* m_rhiFactory;
};

class RenderGraphTexture final : public RenderGraphResource {
  struct SubresourceDesc {
    uint32_t layerBase = 0;
    uint32_t layerCount = 1;
    uint32_t levelBase = 0;
    uint32_t levelCount = 1;

    bool
    operator==(const SubresourceDesc& another) const {
      return another.layerBase == layerBase && another.layerCount == layerCount &&  //
             another.levelBase == levelBase && another.levelCount == levelCount;
    }
  };

  struct SubresourceDesc_Hash {
    size_t
    operator()(const SubresourceDesc& desc) const {
      return std::hash<uint32_t>()(desc.layerBase) + std::hash<uint32_t>()(desc.layerCount) +
             std::hash<uint32_t>()(desc.levelBase) + std::hash<uint32_t>()(desc.levelCount);
    }
  };

 public:
  RenderGraphTexture(const char* name, RHIFactory* rhiFactory, const ImageCreateInfo& createInfo);

  ~RenderGraphTexture();

  void
  Create() override;

  bool
  IsCreate() override {
    return m_image == nullptr;
  }

  ImageView*
  GetImageView(uint32_t layer = 0, uint32_t layerCount = 1, uint32_t levelBase = 0, uint32_t levelCount = 1);

  Image*
  GetImage() {
    return m_image;
  }

 private:
  bool m_isCreate = false;
  Image* m_image = nullptr;
  ImageCreateInfo m_imageCreateInfo;
  HashMap<SubresourceDesc, ImageView*, SubresourceDesc_Hash> m_imageViews;
};

};  // namespace details

template <typename ResourceType>
struct ResourceHandlerBase {
  uint32_t index = 0;
};

using RenderGraphTextureHandler = ResourceHandlerBase<details::RenderGraphTexture>;

}  // namespace Marbas
