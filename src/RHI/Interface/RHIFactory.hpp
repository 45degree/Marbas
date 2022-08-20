#pragma once

#include <GLFW/glfw3.h>

#include <memory>

#include "RHI/Interface/Command.hpp"
#include "RHI/Interface/CommandBuffer.hpp"
#include "RHI/Interface/CommandFactory.hpp"
#include "RHI/Interface/DrawBatch.hpp"
#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/Interface/ImguiInterface.hpp"
#include "RHI/Interface/Pipeline.hpp"
#include "RHI/Interface/Shader.hpp"
#include "RHI/Interface/ShaderStage.hpp"
#include "RHI/Interface/SwapChain.hpp"
#include "RHI/Interface/UniformBuffer.hpp"
#include "RHI/Interface/VertexBuffer.hpp"
#include "RHI/Interface/Viewport.hpp"

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

  [[nodiscard]] virtual std::unique_ptr<Texture2D>
  CreateTexutre2D(const Path& imagePath, uint32_t level) const = 0;

  [[nodiscard]] virtual std::unique_ptr<Texture2D>
  CreateTexutre2D(int width, int height, uint32_t level, TextureFormat formatType) const = 0;

  [[nodiscard]] virtual std::unique_ptr<TextureCubeMap>
  CreateTextureCubeMap(int width, int height, TextureFormat format) const = 0;

  [[nodiscard]] virtual std::unique_ptr<TextureCubeMap>
  CreateTextureCubeMap(const CubeMapCreateInfo& createInfo) const = 0;

  [[nodiscard]] virtual std::unique_ptr<CommandFactory>
  CreateCommandFactory() const = 0;

  [[nodiscard]] virtual std::unique_ptr<UniformBuffer>
  CreateUniformBuffer(uint32_t size) const = 0;

  [[nodiscard]] virtual std::unique_ptr<DynamicUniformBuffer>
  CreateDynamicUniforBuffer(uint32_t size) const = 0;

  [[nodiscard]] virtual std::unique_ptr<FrameBuffer>
  CreateFrameBuffer(const FrameBufferInfo& info) const = 0;

  [[nodiscard]] virtual std::unique_ptr<DescriptorSet>
  CreateDescriptorSet(const DescriptorSetInfo& createInfo) const = 0;

  [[nodiscard]] virtual std::unique_ptr<DynamicDescriptorSet>
  CreateDynamicDescriptorSet(const Vector<uint16_t>& bindingPoints) const = 0;

 public:
  static RHIFactory*
  GetInstance(const RendererType& rendererType);

 private:
  static std::unique_ptr<RHIFactory> m_rhiFactory;

 protected:
  GLFWwindow* m_glfwWindow = nullptr;
};

}  // namespace Marbas
