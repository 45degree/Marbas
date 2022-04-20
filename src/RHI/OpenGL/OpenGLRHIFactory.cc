#include <GLFW/glfw3.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#ifdef WIN32
#include <GL/GL.h>
#else
#include <GL/gl.h>
#endif

#include "RHI/OpenGL/OpenGLRHIFactory.hpp"
#include "RHI/OpenGL/OpenGLFrameBuffer.hpp"
#include "RHI/OpenGL/OpenGLIndexBuffer.hpp"
#include "RHI/OpenGL/OpenGLShader.hpp"
#include "RHI/OpenGL/OpenGLVertexBuffer.hpp"
#include "RHI/OpenGL/OpenGLVertexArray.hpp"
#include "RHI/OpenGL/OpenGLShaderCode.hpp"
#include "RHI/OpenGL/OpenGLViewport.hpp"
#include "RHI/OpenGL/OpenGLDrawBatch.hpp"

#include <folly/Hash.h>
#include <glog/logging.h>
#include <iostream>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Marbas {

// TODO: need to make it more readable
static void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                          GLsizei length, const char *message, const void *userParam) {

    // ignore these non-significant error codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

void OpenGLRHIFactory::Enable(EnableItem item) const {
    switch(item) {
    case EnableItem::DEPTH:
        glEnable(GL_DEPTH_TEST); break;
    case EnableItem::BLEND:
        glEnable(GL_BLEND); break;
    }
}

void OpenGLRHIFactory::Disable(EnableItem item) const {
    switch(item) {
    case EnableItem::DEPTH:
        glDisable(GL_DEPTH_TEST); break;
    case EnableItem::BLEND:
        glDisable(GL_BLEND); break;
    }
}

void OpenGLRHIFactory::ClearColor(float r, float g, float b, float a) const {
    glClearColor(r, g, b ,a);
}

void OpenGLRHIFactory::PrintRHIInfo() const {
    auto vendor = glGetString(GL_VENDOR);
    auto version = glGetString(GL_VERSION);

    LOG(INFO) <<"vendor is " << vendor;
    LOG(INFO) <<"version is " << version;
}

OpenGLRHIFactory::OpenGLRHIFactory() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

void OpenGLRHIFactory::Init(GLFWwindow *window) const {
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        LOG(ERROR) << "fail to initialize glew ";
    }

    // enable OpenGL debug context if context allows for debug context
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        LOG(INFO) << "enable debug";
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    else {
        LOG(INFO) << "disable debug";
    }
}

void OpenGLRHIFactory::SwapBuffer(GLFWwindow* window) const {
    glfwSwapBuffers(window);
}

void OpenGLRHIFactory::ClearBuffer(const ClearBuferBit bufferBit) const {
    switch (bufferBit) {
    case ClearBuferBit::COLOR_BUFFER:
        glClear(GL_COLOR_BUFFER_BIT);
        break;
    case ClearBuferBit::DEPTH_BUFFER:
        glClear(GL_DEPTH_BUFFER_BIT);
        break;
    }
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

std::unique_ptr<VertexArray>
OpenGLRHIFactory::CreateVertexArray() const {
    return std::make_unique<OpenGLVertexArray>();
}

std::unique_ptr<IndexBuffer>
OpenGLRHIFactory::CreateIndexBuffer(const Vector<uint32_t>& indices) const {
    return std::make_unique<OpenGLIndexBuffer>(indices);
}

std::unique_ptr<IndexBuffer>
OpenGLRHIFactory::CreateIndexBuffer(size_t size) const {
    return std::make_unique<OpenGLIndexBuffer>(size);
}

std::unique_ptr<ShaderCode>
OpenGLRHIFactory::CreateShaderCode(const Path& path, const ShaderCodeType type,
                                        const ShaderType shaderType) const {
    auto shaderCode = std::make_unique<OpenGLShaderCode>(shaderType);
    if(type == ShaderCodeType::FILE) {
        shaderCode->ReadFromSource(path);
    }
    else if (type == ShaderCodeType::BINARY) {
        shaderCode->ReadSPIR_V(path, "main");
    }

    return shaderCode;
}

std::unique_ptr<Shader>
OpenGLRHIFactory::CreateShader() const {
    return std::make_unique<OpenGLShader>();
}

std::unique_ptr<Texture2D>
OpenGLRHIFactory::CreateTexutre2D(const Path& imagePath) {
    String pathStr = imagePath.string();

    // load image
    int width, height, nrChannels;
    TextureFormatType formatType;

    auto filename = imagePath.string();
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if(data == nullptr) {
        LOG(ERROR) << FORMAT("failed to load image from {}", filename);
        return nullptr;
    }

    // get hash code of the image
    auto hashCode = folly::hash::fnv32_buf(data, static_cast<size_t>(width) * height * nrChannels);

    formatType = nrChannels == 4 ? TextureFormatType::RGBA : TextureFormatType::RGB;

    // create textrue
    auto texture = std::make_unique<OpenGLTexture2D>(width, height, formatType);
    texture->SetData(data, width * height * nrChannels);
    texture->SetImageInfo(imagePath.string(), hashCode);

    stbi_image_free(data);

    LOG(INFO) << FORMAT("create a opengl texture, the image is {}", pathStr);

    return texture;
}

std::unique_ptr<Texture2D>
OpenGLRHIFactory::CreateTexutre2D(int width, int height, TextureFormatType format) {
    return std::make_unique<OpenGLTexture2D>(width, height, format);
    // auto texturePtr = texture.get();
    //
    // m_texturePool.AddTexture(std::move(texture));

}


void OpenGLRHIFactory::DestoryTexture2D(Texture2D* texture) {
    auto openGLTexture2D = dynamic_cast<OpenGLTexture2D*>(texture);
    m_texturePool.DeleteTexture(openGLTexture2D);
}

std::unique_ptr<DrawBatch>
OpenGLRHIFactory::CreateDrawBatch() const {
    return std::make_unique<OpenGLDrawBatch>();
}

std::unique_ptr<UniformBuffer>
OpenGLRHIFactory::CreateUniformBuffer(uint32_t size, uint32_t bindingPoint) const {
    return std::make_unique<OpenGLUniformBuffer>(size, bindingPoint);
}

std::unique_ptr<Viewport>
OpenGLRHIFactory::CreateViewport() const {
    return std::make_unique<OpenGLViewport>();
}

}  // namespace Marbas
