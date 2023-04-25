#pragma once

#include "AssetManager/TextureAsset.hpp"
#include "GPUDataPipeline.hpp"

namespace Marbas {

class TextureGPUData : public GPUDataPipelineDataBase {
  using Asset = TextureAsset;

 public:
  ImageView*
  GetImageView(uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseLevel = 0, uint32_t levelCount = 1);

  ImTextureID
  GetImGuiTextureId(uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseLevel = 0, uint32_t levelCount = 1);

  ~TextureGPUData();

 public:
  Task<>
  Load(const Asset& asset);

  Task<>
  Update(const Asset& asset);

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
  HashMap<SubresourceDesc, ImageView*, SubresourceDesc_Hash> m_imageViews;
  HashMap<SubresourceDesc, ImTextureID, SubresourceDesc_Hash> m_imguiTextures;
};

using TextureGPUDataManager = Singleton<GPUDataPipelineFromAssetBase<TextureGPUData>>;

}  // namespace Marbas
