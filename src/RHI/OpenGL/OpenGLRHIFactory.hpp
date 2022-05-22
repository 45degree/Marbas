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
  Init() const override;

  [[nodiscard]] std::shared_ptr<ImguiInterface>
  CreateImguiInterface() const override;

  [[nodiscard]] std::shared_ptr<VertexBuffer>
  CreateVertexBuffer(const void* data, size_t size) const override;

  [[nodiscard]] std::shared_ptr<VertexBuffer>
  CreateVertexBuffer(size_t size) const override;

  [[nodiscard]] std::shared_ptr<IndexBuffer>
  CreateIndexBuffer(const Vector<uint32_t>& indices) const override;

  [[nodiscard]] std::shared_ptr<ShaderStage>
  CreateShaderStage(const Path& path, const ShaderCodeType type,
                    const ShaderType shaderType) const override;

  [[nodiscard]] std::shared_ptr<Shader>
  CreateShader() const override;

  [[nodiscard]] std::shared_ptr<SwapChain>
  CreateSwapChain() override;

  [[nodiscard]] std::shared_ptr<RenderPass>
  CreateRenderPass(const RenderPassCreateInfo& createInfo) override;

  [[nodiscard]] std::shared_ptr<GraphicsPipeLine>
  CreateGraphicsPipeLine() override;

  [[nodiscard]] std::shared_ptr<Texture2D>
  CreateTexutre2D(const Path& imagePath) const override;

  [[nodiscard]] std::shared_ptr<Texture2D>
  CreateTexutre2D(int width, int height, TextureFormat formatType) const override;

  [[nodiscard]] std::shared_ptr<TextureCubeMap>
  CreateTextureCubeMap(int width, int height, TextureFormat format) const override;

  [[nodiscard]] std::shared_ptr<TextureCubeMap>
  CreateTextureCubeMap(const CubeMapCreateInfo& createInfo) const override;

  [[nodiscard]] std::shared_ptr<UniformBuffer>
  CreateUniformBuffer(uint32_t size) const override;

  [[nodiscard]] std::shared_ptr<DynamicUniformBuffer>
  CreateDynamicUniforBuffer(uint32_t size) const override;

  [[nodiscard]] std::shared_ptr<FrameBuffer>
  CreateFrameBuffer(const FrameBufferInfo& info) const override;

  [[nodiscard]] std::shared_ptr<CommandFactory>
  CreateCommandFactory() const override;

  [[nodiscard]] std::shared_ptr<DescriptorSet>
  CreateDescriptorSet(const DescriptorSetInfo& info) const override;

  [[nodiscard]] std::shared_ptr<DynamicDescriptorSet>
  CreateDynamicDescriptorSet(const Vector<uint16_t>& bindingPoints) const override;

 private:
  friend RHIFactory;
};

}  // namespace Marbas
