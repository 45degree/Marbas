#pragma once

#include <GLFW/glfw3.h>

#include <memory>

#include "RHI/Interface/CommandBuffer.hpp"
#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/Interface/ImguiInterface.hpp"
#include "RHI/Interface/Pipeline.hpp"
#include "RHI/Interface/Shader.hpp"
#include "RHI/Interface/ShaderStage.hpp"
#include "RHI/Interface/SwapChain.hpp"
#include "RHI/Interface/UniformBuffer.hpp"
#include "RHI/Interface/VertexBuffer.hpp"

namespace Marbas {

enum class RendererType {
  OPENGL,
  VULKAN,
};

enum class ClearBuferBit {
  COLOR_BUFFER,
  DEPTH_BUFFER,
};
enum class EnableItem {
  DEPTH,
};

struct CubeMapCreateInfo {
  Path top;
  Path bottom;
  Path back;
  Path front;
  Path left;
  Path right;
};

struct OpenGLRHICreateInfo {
  bool useSPIRV = false;
};

struct RHICreateInfo {
  OpenGLRHICreateInfo m_openglRHICreateInfo;
};

/**
 * @brief
 */
class RHIFactory {
 protected:
  RHIFactory() {}

 public:
  RHIFactory(const RHIFactory&) = delete;
  RHIFactory&
  operator=(const RHIFactory&) = delete;

  virtual ~RHIFactory() = default;

 public:
  void
  SetGLFWwindow(GLFWwindow* glfwWindow) {
    m_glfwWindow = glfwWindow;
  }

  virtual void
  Init(const RHICreateInfo& extraInfo) const = 0;

  [[nodiscard]] virtual std::unique_ptr<ImguiInterface>
  CreateImguiInterface() const = 0;

  [[nodiscard]] virtual std::unique_ptr<VertexBuffer>
  CreateVertexBuffer(const void* data, size_t size) const = 0;

  [[nodiscard]] virtual std::unique_ptr<VertexBuffer>
  CreateVertexBuffer(size_t size) const = 0;

  [[nodiscard]] virtual std::unique_ptr<IndexBuffer>
  CreateIndexBuffer(const Vector<uint32_t>& indices) const = 0;

  [[nodiscard]] virtual std::unique_ptr<ShaderStage>
  CreateShaderStage(const ShaderType shaderType) const = 0;

  [[nodiscard]] virtual std::unique_ptr<Shader>
  CreateShader() const = 0;

  [[nodiscard]] virtual std::unique_ptr<SwapChain>
  CreateSwapChain() = 0;

  [[nodiscard]] virtual std::unique_ptr<RenderPass>
  CreateRenderPass(const RenderPassCreateInfo& createInfo) = 0;

  [[nodiscard]] virtual std::unique_ptr<GraphicsPipeLine>
  CreateGraphicsPipeLine() = 0;

  [[nodiscard]] virtual std::unique_ptr<Texture>
  CreateTexture(const ImageDesc& imageDesc) const = 0;

  [[nodiscard]] virtual std::unique_ptr<Texture>
  CreateTexture2D(const Path& imagePath, uint32_t levels, bool isHDR) const = 0;

  [[nodiscard]] virtual std::unique_ptr<Texture>
  CreateTextureCubeMap(const CubeMapCreateInfo& createInfo, uint32_t levels) const = 0;

  [[nodiscard]] virtual std::unique_ptr<ImageView>
  CreateImageView(const ImageViewDesc& imageViewDesc) const = 0;

  [[nodiscard]] virtual std::unique_ptr<CommandBuffer>
  CreateCommandBuffer() const = 0;

  [[nodiscard]] virtual std::unique_ptr<UniformBuffer>
  CreateUniformBuffer(uint32_t size) const = 0;

  [[nodiscard]] virtual std::unique_ptr<DynamicUniformBuffer>
  CreateDynamicUniforBuffer(uint32_t size) const = 0;

  [[nodiscard]] virtual std::unique_ptr<FrameBuffer>
  CreateFrameBuffer(const FrameBufferInfo& info) const = 0;

  [[nodiscard]] virtual std::unique_ptr<DescriptorSet>
  CreateDescriptorSet(const DescriptorSetLayout& createInfo) const = 0;

 public:
  static RHIFactory*
  GetInstance(const RendererType& rendererType);

 private:
  static std::unique_ptr<RHIFactory> m_rhiFactory;

 protected:
  GLFWwindow* m_glfwWindow = nullptr;
};

}  // namespace Marbas
