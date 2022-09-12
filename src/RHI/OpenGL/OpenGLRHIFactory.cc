#include <GLFW/glfw3.h>

#include "OpenGLSwapChain.hpp"
#include "RHI/OpenGL/OpenGLCommandBuffer.hpp"
#include "RHI/OpenGL/OpenGLDescriptorSet.hpp"
#include "RHI/OpenGL/OpenGLImguiInterface.hpp"
#include "RHI/OpenGL/OpenGLPipeline.hpp"
#include "RHI/OpenGL/OpenGLRenderPass.hpp"

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#ifdef WIN32
#include <GL/GL.h>
#else
#include <GL/gl.h>
#endif

#include <folly/Hash.h>
#include <glog/logging.h>

#include <iostream>

#include "RHI/OpenGL/OpenGLFrameBuffer.hpp"
#include "RHI/OpenGL/OpenGLIndexBuffer.hpp"
#include "RHI/OpenGL/OpenGLRHIFactory.hpp"
#include "RHI/OpenGL/OpenGLShader.hpp"
#include "RHI/OpenGL/OpenGLShaderStage.hpp"
#include "RHI/OpenGL/OpenGLVertexBuffer.hpp"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Marbas {

// TODO: need to make it more readable
static void
glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
              const char* message, const void* userParam) {
  // ignore these non-significant error codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "Source: API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "Source: Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "Source: Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "Source: Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "Source: Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "Source: Other";
      break;
  }
  std::cout << std::endl;

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "Type: Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "Type: Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "Type: Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "Type: Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "Type: Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cout << "Type: Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cout << "Type: Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cout << "Type: Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      std::cout << "Type: Other";
      break;
  }
  std::cout << std::endl;

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "Severity: high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "Severity: medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "Severity: low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "Severity: notification";
      break;
  }
  std::cout << std::endl;
  std::cout << std::endl;
}

OpenGLRHIFactory::OpenGLRHIFactory() : RHIFactory() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

void
OpenGLRHIFactory::Init(const RHICreateInfo& createInfo) const {
  m_enableSpriv = createInfo.m_openglRHICreateInfo.useSPIRV;

  glfwMakeContextCurrent(m_glfwWindow);
  glfwSwapInterval(1);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    LOG(ERROR) << "fail to initialize glew ";
  }

  // enable OpenGL debug context if context allows for debug context
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    LOG(INFO) << "enable debug";
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);  // makes sure errors are displayed synchronously
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  } else {
    LOG(INFO) << "disable debug";
  }

  // check extension
  if (!glfwExtensionSupported("GL_ARB_gl_spirv") && m_enableSpriv) {
    // The extension is supported by the current context
    LOG(WARNING) << "opengl dont't support spirv";
    m_enableSpriv = false;
  }
}

std::unique_ptr<ImguiInterface>
OpenGLRHIFactory::CreateImguiInterface() const {
  return std::make_unique<OpenGLImguiInterface>();
}

std::unique_ptr<FrameBuffer>
OpenGLRHIFactory::CreateFrameBuffer(const FrameBufferInfo& info) const {
  return std::make_unique<OpenGLFrameBuffer>(info);
}

std::unique_ptr<VertexBuffer>
OpenGLRHIFactory::CreateVertexBuffer(const void* data, size_t size) const {
  return std::make_unique<OpenGLVertexBuffer>(data, size);
}

std::unique_ptr<VertexBuffer>
OpenGLRHIFactory::CreateVertexBuffer(size_t size) const {
  return std::make_unique<OpenGLVertexBuffer>(size);
}

std::unique_ptr<IndexBuffer>
OpenGLRHIFactory::CreateIndexBuffer(const Vector<uint32_t>& indices) const {
  return std::make_unique<OpenGLIndexBuffer>(indices);
}

std::unique_ptr<ShaderStage>
OpenGLRHIFactory::CreateShaderStage(const ShaderType shaderType) const {
  auto shaderStage = std::make_unique<OpenGLShaderStage>(shaderType);
  shaderStage->EnableSpriv(m_enableSpriv);
  return shaderStage;
}

std::unique_ptr<Shader>
OpenGLRHIFactory::CreateShader() const {
  return std::make_unique<OpenGLShader>();
}

std::unique_ptr<Texture>
OpenGLRHIFactory::CreateTexture2D(const Path& imagePath, uint32_t level) const {
  String pathStr = imagePath.string();

  // load image
  int width, height, nrChannels;
  TextureFormat formatType;

  auto filename = imagePath.string();

#ifdef _WIN32
  std::replace(filename.begin(), filename.end(), '/', '\\');
#elif __linux__
  std::replace(filename.begin(), filename.end(), '\\', '/');
#endif

  unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

  if (data == nullptr) {
    LOG(ERROR) << FORMAT("failed to load image from {}", filename);
    return nullptr;
  }

  // get hash code of the image
  // auto hashCode = folly::hash::fnv32_buf(data, static_cast<size_t>(width) * height * nrChannels);

  formatType = nrChannels == 4 ? TextureFormat::RGBA : TextureFormat::RGB;

  ImageDesc desc{
      .textureType = TextureType::TEXTURE2D,
      .format = formatType,
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
      .mipmapLevel = level,
  };

  // create textrue
  auto texture = std::make_unique<OpenGLTexture>(desc);
  texture->SetData(data, width * height * nrChannels);
  // texture->SetImageInfo(imagePath.string(), hashCode);

  stbi_image_free(data);

  LOG(INFO) << FORMAT("create a opengl texture, the image is {}", pathStr);

  return texture;
}

std::unique_ptr<Texture>
OpenGLRHIFactory::CreateTexture(const ImageDesc& imageDesc) const {
  return std::make_unique<OpenGLTexture>(imageDesc);
}

std::unique_ptr<Texture>
OpenGLRHIFactory::CreateTextureCubeMap(const CubeMapCreateInfo& createInfo, uint32_t levels) const {
  int width, height, nrChannels;
  TextureFormat formatType;

  auto* data = stbi_load(createInfo.back.string().c_str(), &width, &height, &nrChannels, 0);

  formatType = nrChannels == 4 ? TextureFormat::RGBA : TextureFormat::RGB;

  ImageDesc desc{
      .textureType = TextureType::CUBEMAP,
      .format = formatType,
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
      .mipmapLevel = levels,
  };

  // create texture cubemap
  auto textureCubeMap = std::make_unique<OpenGLTexture>(desc);

  // Set Data
  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
  textureCubeMap->SetData(data, width * height * nrChannels, 0, 5);
  stbi_image_free(data);

  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
  data = stbi_load(createInfo.front.string().c_str(), &width, &height, &nrChannels, 0);
  textureCubeMap->SetData(data, width * height * nrChannels, 0, 4);
  stbi_image_free(data);

  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
  data = stbi_load(createInfo.bottom.string().c_str(), &width, &height, &nrChannels, 0);
  textureCubeMap->SetData(data, width * height * nrChannels, 0, 3);
  stbi_image_free(data);

  // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
  data = stbi_load(createInfo.top.string().c_str(), &width, &height, &nrChannels, 0);
  textureCubeMap->SetData(data, width * height * nrChannels, 0, 2);
  stbi_image_free(data);

  // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
  data = stbi_load(createInfo.left.string().c_str(), &width, &height, &nrChannels, 0);
  textureCubeMap->SetData(data, width * height * nrChannels, 0, 1);
  stbi_image_free(data);

  // GL_TEXTURE_CUBE_MAP_POSITION_X
  data = stbi_load(createInfo.right.string().c_str(), &width, &height, &nrChannels, 0);
  textureCubeMap->SetData(data, width * height * nrChannels, 0, 0);
  stbi_image_free(data);

  LOG(INFO) << FORMAT(
      "create a cubemap, back: {}\n front: {}\n bottom: {}\n  top: {}\n left: {}\n right: {}",
      createInfo.back.string(), createInfo.front.string(), createInfo.bottom.string(),
      createInfo.top.string(), createInfo.left.string(), createInfo.right.string());

  return textureCubeMap;
}

std::unique_ptr<ImageView>
OpenGLRHIFactory::CreateImageView() const {
  return std::make_unique<OpenGLImageView>();
}

std::unique_ptr<UniformBuffer>
OpenGLRHIFactory::CreateUniformBuffer(uint32_t size) const {
  return std::make_unique<OpenGLUniformBuffer>(size);
}

std::unique_ptr<DynamicUniformBuffer>
OpenGLRHIFactory::CreateDynamicUniforBuffer(uint32_t size) const {
  return std::make_unique<OpenGLDynamicUniformBuffer>(size);
}

std::unique_ptr<CommandBuffer>
OpenGLRHIFactory::CreateCommandBuffer() const {
  return std::make_unique<OpenGLCommandBuffer>();
}

std::unique_ptr<SwapChain>
OpenGLRHIFactory::CreateSwapChain() {
  int width, height;
  glfwGetFramebufferSize(m_glfwWindow, &width, &height);
  return std::make_unique<OpenGLSwapChain>(m_glfwWindow, width, height);
}

std::unique_ptr<RenderPass>
OpenGLRHIFactory::CreateRenderPass(const RenderPassCreateInfo& createInfo) {
  return std::make_unique<OpenGLRenderPass>(createInfo);
}

std::unique_ptr<GraphicsPipeLine>
OpenGLRHIFactory::CreateGraphicsPipeLine() {
  return std::make_unique<OpenGLGraphicsPipeline>();
}

std::unique_ptr<DescriptorSet>
OpenGLRHIFactory::CreateDescriptorSet(const DescriptorSetLayout& info) const {
  return std::make_unique<OpenGLDescriptorSet>(info);
}

}  // namespace Marbas
