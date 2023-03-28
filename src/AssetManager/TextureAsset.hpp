#pragma once

#include <cereal/types/vector.hpp>
#include <vector>

#include "AssetManager/AssetManager.hpp"
#include "AssetManager/GPUAssetUpLoader.hpp"
#include "Common/Common.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct TextureAsset final : public AssetBase {
  std::vector<unsigned char> m_data;
  uint32_t m_width;
  uint32_t m_height;
  ImageFormat m_format;

  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(m_uid, m_width, m_height, m_data, m_format);
  }

  static std::shared_ptr<TextureAsset>
  Load(const Path& m_path, bool flipV = false);
};

struct TextureGPUAsset final : public GPUAssetBase<TextureGPUAsset> {
  friend class GPUAssetBase<TextureGPUAsset>;

 private:
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

  ImageCreateInfo m_imageCreateInfo;
  Image* m_image;
  RHIFactory* m_rhiFactory;
  HashMap<SubresourceDesc, ImageView*, SubresourceDesc_Hash> m_imageViews;
  HashMap<SubresourceDesc, ImTextureID, SubresourceDesc_Hash> m_imguiTextures;

 public:
  ImageView*
  GetImageView(uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseLevel = 0, uint32_t levelCount = 1);

  ImTextureID
  GetImGuiTextureId(uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseLevel = 0, uint32_t levelCount = 1);

  // TODO:
  TextureGPUAsset(RHIFactory* rhiFactory) : m_image(nullptr), m_rhiFactory(rhiFactory){};
  ~TextureGPUAsset();

  static std::shared_ptr<TextureGPUAsset>
  LoadToGPU(const std::shared_ptr<TextureAsset>& asset, RHIFactory* rhiFactory);

 protected:
  void
  Update(const std::shared_ptr<TextureAsset>& asset) {}
};

}  // namespace Marbas
