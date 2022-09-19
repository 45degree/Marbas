#pragma once

#include <unordered_map>

#include "RHI/Interface/RHIFactory.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"

namespace Marbas {

class OpenGLRHIFactory final : public RHIFactory {
 private:
  OpenGLRHIFactory();

 public:
  OpenGLRHIFactory(const OpenGLRHIFactory&) = delete;
  OpenGLRHIFactory&
  operator=(const OpenGLRHIFactory&) = delete;
  ~OpenGLRHIFactory() override = default;

 public:
  void
  Init(const RHICreateInfo& createInfo) const override;

  [[nodiscard]] std::unique_ptr<ImguiInterface>
  CreateImguiInterface() const override;

  [[nodiscard]] std::unique_ptr<VertexBuffer>
  CreateVertexBuffer(const void* data, size_t size) const override;

  [[nodiscard]] std::unique_ptr<VertexBuffer>
  CreateVertexBuffer(size_t size) const override;

  [[nodiscard]] std::unique_ptr<IndexBuffer>
  CreateIndexBuffer(const Vector<uint32_t>& indices) const override;

  [[nodiscard]] std::unique_ptr<ShaderStage>
  CreateShaderStage(const ShaderType shaderType) const override;

  [[nodiscard]] std::unique_ptr<Shader>
  CreateShader() const override;

  [[nodiscard]] std::unique_ptr<SwapChain>
  CreateSwapChain() override;

  [[nodiscard]] std::unique_ptr<RenderPass>
  CreateRenderPass(const RenderPassCreateInfo& createInfo) override;

  [[nodiscard]] std::unique_ptr<GraphicsPipeLine>
  CreateGraphicsPipeLine() override;

  [[nodiscard]] std::unique_ptr<Texture>
  CreateTexture(const ImageDesc& imageDesc) const override;

  std::unique_ptr<Texture>
  CreateTexture2D(const Path& imagePath, uint32_t level, bool isHDR) const override;

  // [[nodiscard]] std::unique_ptr<Texture2D>
  // CreateTexutre2D(const Path& imagePath, uint32_t level) const override;
  //
  // [[nodiscard]] std::unique_ptr<Texture2D>
  // CreateTexutre2D(int width, int height, uint32_t level, TextureFormat formatType) const
  // override;
  //
  // [[nodiscard]] std::unique_ptr<TextureCubeMap>
  // CreateTextureCubeMap(int width, int height, TextureFormat format) const override;
  //
  [[nodiscard]] std::unique_ptr<Texture>
  CreateTextureCubeMap(const CubeMapCreateInfo& createInfo, uint32_t level) const override;

  [[nodiscard]] std::unique_ptr<ImageView>
  CreateImageView() const override;

  [[nodiscard]] std::unique_ptr<UniformBuffer>
  CreateUniformBuffer(uint32_t size) const override;

  [[nodiscard]] std::unique_ptr<DynamicUniformBuffer>
  CreateDynamicUniforBuffer(uint32_t size) const override;

  [[nodiscard]] std::unique_ptr<FrameBuffer>
  CreateFrameBuffer(const FrameBufferInfo& info) const override;

  [[nodiscard]] std::unique_ptr<CommandBuffer>
  CreateCommandBuffer() const override;

  [[nodiscard]] std::unique_ptr<DescriptorSet>
  CreateDescriptorSet(const DescriptorSetLayout& info) const override;

  // [[nodiscard]] std::unique_ptr<DynamicDescriptorSet>
  // CreateDynamicDescriptorSet(const Vector<uint16_t>& bindingPoints) const override;

 private:
  mutable bool m_enableSpriv = true;
  friend RHIFactory;
};

}  // namespace Marbas
