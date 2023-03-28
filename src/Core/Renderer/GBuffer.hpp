#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct GBuffer_Color {
  constexpr static StringView typeName = "Color";
  constexpr static ImageFormat format = ImageFormat::RGBA;
  constexpr static ImageViewType type = ImageViewType::TEXTURE2D;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_Position {
  constexpr static StringView typeName = "Position";
  constexpr static ImageFormat format = ImageFormat::RGBA32F;
  constexpr static ImageViewType type = ImageViewType::TEXTURE2D;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_Normals {
  constexpr static StringView typeName = "Normal";
  constexpr static ImageFormat format = ImageFormat::RGBA32F;
  constexpr static ImageViewType type = ImageViewType::TEXTURE2D;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_Depth {
  constexpr static StringView typeName = "Depth";
  constexpr static ImageFormat format = ImageFormat::DEPTH;
  constexpr static ImageViewType type = ImageViewType::TEXTURE2D;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_AmbientOcclusion {
  constexpr static StringView typeName = "Ao";
  constexpr static ImageFormat format = ImageFormat::R32F;
  constexpr static ImageViewType type = ImageViewType::TEXTURE2D;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_Roughness {
  constexpr static StringView typeName = "roughness";
  constexpr static ImageFormat format = ImageFormat::R32F;
  constexpr static ImageViewType type = ImageViewType::TEXTURE2D;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_Metallic {
  constexpr static StringView typeName = "Metallic";
  constexpr static ImageFormat format = ImageFormat::R32F;
  constexpr static ImageViewType type = ImageViewType::TEXTURE2D;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_HDR {
  constexpr static StringView typeName = "HDR";
  constexpr static ImageFormat format = ImageFormat::RGBA16F;
  constexpr static ImageViewType type = ImageViewType::CUBEMAP;
  constexpr static uint32_t layers = 6;
  constexpr static uint32_t levels = 1;
  constexpr static uint32_t height = 512;
  constexpr static uint32_t width = 512;
};

struct GBuffer_PreFilterCubemap {
  constexpr static StringView typeName = "PreFilterCubeMap";
  constexpr static ImageFormat format = ImageFormat::RGBA16F;
  constexpr static ImageViewType type = ImageViewType::CUBEMAP;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_IBL_BRDF_LOD {
  constexpr static StringView typeName = "IblBrdfLod";
  constexpr static ImageFormat format = ImageFormat::RGBA16F;
  constexpr static ImageViewType type = ImageViewType::CUBEMAP;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_ShadowMap {
  constexpr static StringView typeName = "ShadowMap";
  constexpr static ImageFormat format = ImageFormat::DEPTH;
  constexpr static ImageViewType type = ImageViewType::TEXTURE2D_ARRAY;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBuffer_ShadowMapCube {
  constexpr static StringView typeName = "ShadowMapCube";
  constexpr static ImageFormat format = ImageFormat::DEPTH;
  constexpr static ImageViewType type = ImageViewType::CUBEMAP_ARRAY;
  constexpr static uint32_t layers = 1;
  constexpr static uint32_t levels = 1;
};

struct GBufferMetaInfo {
  ImageFormat format;
  ImageViewType imageViewType;
  uint32_t levels = 1;
  uint32_t layers = 1;
  std::optional<uint32_t> width;
  std::optional<uint32_t> height;
};

class GBuffer final {
 public:
  GBuffer(uint32_t width, uint32_t height, uint32_t usage, RHIFactory* rhiFactory, const GBufferMetaInfo& metaInfo);
  ~GBuffer();

 public:
  void
  Recreate(uint32_t width, uint32_t height, uint32_t usage, const GBufferMetaInfo& metaInfo);

  Image*
  GetImage() const {
    return m_texture;
  }

  ImageView*
  GetWholeImageView() const {
    return m_wholeImageView;
  }

 private:
  ImageViewType m_imageViewType;
  Image* m_texture = nullptr;
  ImageView* m_wholeImageView = nullptr;

  BufferContext* m_bufferContext = nullptr;
  uint32_t m_width;
  uint32_t m_height;
};

class GBufferRegistry final {
  define_has_member(width);
  define_has_member(height);

 public:
  GBufferRegistry();
  ~GBufferRegistry() = default;

 public:
  template <typename T>
  constexpr void
  Emplace() {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    GBufferMetaInfo metaInfo{
        .format = T::format,
        .imageViewType = T::type,
        .levels = T::levels,
        .layers = T::layers,
    };

    if constexpr (has_member(T, width)) {
      metaInfo.width = T::width;
    }
    if constexpr (has_member(T, height)) {
      metaInfo.height = T::height;
    }

    Emplace(String(T::typeName), metaInfo);
  }

  void
  Emplace(const String& typeName, const GBufferMetaInfo& metaInfo) {
    if (m_metaInfo.find(typeName) != m_metaInfo.end()) {
      throw std::runtime_error(FORMAT("{} has beed registred in the registry", typeName));
    }
    m_metaInfo.insert({typeName, metaInfo});
  }

  std::shared_ptr<GBuffer>
  CreateGBuffer(RHIFactory* rhiFactory, const String& gbufferTypeName, int width, int height, uint32_t usage) {
    const auto& metaInfo = m_metaInfo[gbufferTypeName];
    return std::make_shared<GBuffer>(width, height, usage, rhiFactory, metaInfo);
  }

  const GBufferMetaInfo&
  GetGBufferMetaInfo(const String& name) const {
    return m_metaInfo.at(name);
  }

  GBufferMetaInfo&
  GetGBufferMetaInfo(const String& name) {
    return m_metaInfo.at(name);
  }

 private:
  HashMap<String, GBufferMetaInfo> m_metaInfo;
};

}  // namespace Marbas
