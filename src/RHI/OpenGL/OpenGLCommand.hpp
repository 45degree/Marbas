#pragma once

#include <glog/logging.h>

#include "RHI/Interface/CommandBuffer.hpp"
#include "RHI/OpenGL/OpenGLDescriptorSet.hpp"
#include "RHI/OpenGL/OpenGLIndexBuffer.hpp"
#include "RHI/OpenGL/OpenGLPipeline.hpp"
#include "RHI/OpenGL/OpenGLRenderPass.hpp"
#include "RHI/OpenGL/OpenGLVertexBuffer.hpp"

namespace Marbas {

template <typename Derive>
class IOpenGLCommand {
 public:
  void
  Execute() const {
    static_cast<Derive&>(*this)->Execute();
  }

  void
  OnEndRenderPass() const {
    static_cast<Derive&>(*this)->OnEndRenderPass();
  }
};

class OpenGLBindDescriptorSet final : public IOpenGLCommand<OpenGLBindDescriptorSet> {
 public:
  OpenGLBindDescriptorSet() = default;
  OpenGLBindDescriptorSet(const OpenGLBindDescriptorSet& command)
      : m_openGLDescriptor(command.m_openGLDescriptor), m_bufferPiece(command.m_bufferPiece) {}
  ~OpenGLBindDescriptorSet() = default;

 public:
  void
  SetDescriptor(const std::shared_ptr<DescriptorSet>& descriptorSet) {
    auto openglDescriptorSet = std::static_pointer_cast<OpenGLDescriptorSet>(descriptorSet);
    DLOG_ASSERT(openglDescriptorSet != nullptr) << "can't set descriptorSet";
    m_openGLDescriptor = openglDescriptorSet;
  }

  void
  SetDescriptorLayout(const DescriptorSetLayout& layouts) {}

  void
  SetDynamicDescriptorBufferPiece(const Vector<DynamicBufferPiece>& bufferPiece) {
    m_bufferPiece = bufferPiece;
  }

 public:
  void
  Execute() const {
    m_openGLDescriptor->Bind(m_bufferPiece);
  }

  void
  OnEndRenderPass() const {
    m_openGLDescriptor->UnBind();
  }

 private:
  std::shared_ptr<OpenGLDescriptorSet> m_openGLDescriptor = nullptr;
  Vector<DynamicBufferPiece> m_bufferPiece = {};
};

class OpenGLBeginRenderPass final : public IOpenGLCommand<OpenGLBeginRenderPass> {
 public:
  OpenGLBeginRenderPass() = default;
  OpenGLBeginRenderPass(const OpenGLBeginRenderPass& command)
      : m_renderPass(command.m_renderPass), m_frameBuffer(command.m_frameBuffer) {}
  virtual ~OpenGLBeginRenderPass() = default;

 public:
  void
  SetRenderPass(const std::shared_ptr<OpenGLRenderPass>& renderPass) {
    m_renderPass = renderPass;

    LOG_IF(ERROR, m_renderPass == nullptr) << "render pass is not a opengl render pass";
  }

  void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) {
    m_frameBuffer = std::dynamic_pointer_cast<OpenGLFrameBuffer>(frameBuffer);

    LOG_IF(ERROR, m_frameBuffer == nullptr) << "frame buffer is not a opengl frame buffer";
  }

  void
  SetClearColor(const std::array<float, 4>& clearColor) {
    m_clearColor = clearColor;
  }

  void
  Execute() const {
    if (m_clearColor.has_value()) {
      const auto& value = m_clearColor.value();
      glClearColor(value[0], value[1], value[2], value[3]);
    }
    m_frameBuffer->Bind();
    m_renderPass->Bind();
  }

  void
  OnEndRenderPass() const {}

 private:
  std::shared_ptr<OpenGLRenderPass> m_renderPass = nullptr;
  std::shared_ptr<OpenGLFrameBuffer> m_frameBuffer = nullptr;
  std::optional<std::array<float, 4>> m_clearColor = std::nullopt;
};

class OpenGLEndRenderpass final : public IOpenGLCommand<OpenGLEndRenderpass> {
 public:
  OpenGLEndRenderpass() = default;
  OpenGLEndRenderpass(const OpenGLEndRenderpass& command)
      : m_renderPass(command.m_renderPass), m_frameBuffer(command.m_frameBuffer) {}
  virtual ~OpenGLEndRenderpass() = default;

 public:
  void
  Execute() const {
    if (m_frameBuffer != nullptr) {
      m_frameBuffer->UnBind();
    }
  }

  void
  SetRenderPass(const std::shared_ptr<RenderPass>& renderPass) {
    m_renderPass = std::static_pointer_cast<OpenGLRenderPass>(renderPass);

    LOG_IF(ERROR, m_renderPass == nullptr) << "render pass is not a opengl render pass";
  }

  void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) {
    m_frameBuffer = std::dynamic_pointer_cast<OpenGLFrameBuffer>(frameBuffer);

    LOG_IF(ERROR, m_frameBuffer == nullptr) << "frame buffer is not a opengl frame buffer";
  }

  void
  OnEndRenderPass() const {}

 private:
  std::shared_ptr<OpenGLRenderPass> m_renderPass = nullptr;
  std::shared_ptr<OpenGLFrameBuffer> m_frameBuffer = nullptr;
};

class OpenGLBindPipeline final : public IOpenGLCommand<OpenGLBindPipeline> {
 public:
  OpenGLBindPipeline() = default;
  OpenGLBindPipeline(const OpenGLBindPipeline& command)
      : m_graphicsPipeline(command.m_graphicsPipeline) {}
  virtual ~OpenGLBindPipeline() = default;

 public:
  void
  SetPipeLine(const std::shared_ptr<OpenGLGraphicsPipeline>& graphicsPipeline) {
    m_graphicsPipeline = graphicsPipeline;
    LOG_IF(ERROR, m_graphicsPipeline == nullptr)
        << "graphicsPipeline is not an opengl graphics pipeline";
  }

 public:
  void
  Execute() const {
    if (m_graphicsPipeline != nullptr) {
      m_graphicsPipeline->Bind();
    }
  }

  void
  OnEndRenderPass() const {
    if (m_graphicsPipeline != nullptr) {
      m_graphicsPipeline->UnBind();
    }
  }

 private:
  std::shared_ptr<OpenGLGraphicsPipeline> m_graphicsPipeline = nullptr;
};

class OpenGLBindVertexBuffer final : public IOpenGLCommand<OpenGLBindVertexBuffer> {
 public:
  OpenGLBindVertexBuffer() = default;
  OpenGLBindVertexBuffer(const OpenGLBindVertexBuffer& command)
      : m_vertexBuffer(command.m_vertexBuffer) {}
  virtual ~OpenGLBindVertexBuffer() = default;

 public:
  void
  SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
    m_vertexBuffer = std::static_pointer_cast<OpenGLVertexBuffer>(vertexBuffer);
    LOG_IF(ERROR, m_vertexBuffer == nullptr) << "vertex buffer is not an opengl vertex buffer";
  }

  void
  Execute() const {
    if (m_vertexBuffer != nullptr) {
      m_vertexBuffer->Bind();
    }
  }

  void
  OnEndRenderPass() const {}

 private:
  std::shared_ptr<OpenGLVertexBuffer> m_vertexBuffer;
};

class OpenGLBindIndexBuffer final : public IOpenGLCommand<OpenGLBindIndexBuffer> {
 public:
  OpenGLBindIndexBuffer() = default;
  OpenGLBindIndexBuffer(const OpenGLBindIndexBuffer& command)
      : m_indexBuffer(command.m_indexBuffer) {}
  virtual ~OpenGLBindIndexBuffer() = default;

 public:
  void
  SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
    m_indexBuffer = std::static_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
    LOG_IF(ERROR, m_indexBuffer == nullptr) << "index buffer is not an opengl index buffer";
  }

  void
  Execute() const {
    m_indexBuffer->Bind();
  }

  void
  OnEndRenderPass() const {}

 private:
  std::shared_ptr<OpenGLIndexBuffer> m_indexBuffer = nullptr;
};

class OpenGLDrawIndex final : public IOpenGLCommand<OpenGLDrawIndex> {
 public:
  OpenGLDrawIndex() = default;
  OpenGLDrawIndex(const OpenGLDrawIndex& command)
      : m_indexCount(command.m_indexCount),
        m_instanceCount(command.m_instanceCount),
        m_pipeline(command.m_pipeline) {}
  virtual ~OpenGLDrawIndex() = default;

 public:
  void
  SetPipeline(const std::shared_ptr<OpenGLGraphicsPipeline>& pipeline) {
    m_pipeline = pipeline;
  }

  void
  SetIndexCount(int count) {
    m_indexCount = count;
  }

  void
  SetInstanceCount(int count) {
    m_instanceCount = count;
  }

  void
  Execute() const {
    if (m_pipeline->GetVertexInputRate() == VertexInputRate::INSTANCE) {
      glDrawElementsInstanced(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0, m_instanceCount);
    } else {
      glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    }
  }

  void
  OnEndRenderPass() const {}

 private:
  std::shared_ptr<OpenGLGraphicsPipeline> m_pipeline;
  uint32_t m_indexCount = 0;
  uint32_t m_instanceCount = 0;
};

class OpenGLDrawArray final : public IOpenGLCommand<OpenGLDrawArray> {
 public:
  OpenGLDrawArray() = default;
  OpenGLDrawArray(const OpenGLDrawArray& command)
      : m_vertexCount(command.m_vertexCount),
        m_instanceCount(command.m_instanceCount),
        m_pipeline(command.m_pipeline) {}
  virtual ~OpenGLDrawArray() = default;

 public:
  void
  SetPipeline(const std::shared_ptr<OpenGLGraphicsPipeline>& pipeline) {
    m_pipeline = pipeline;
  }

  void
  SetVertexCount(int count) {
    m_vertexCount = count;
  }

  void
  SetInstanceCount(int count) {
    m_instanceCount = count;
  }

  void
  Execute() const {
    if (m_pipeline->GetVertexInputRate() == VertexInputRate::INSTANCE) {
      glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, m_instanceCount);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }
  }

  void
  OnEndRenderPass() const {}

 private:
  std::shared_ptr<OpenGLGraphicsPipeline> m_pipeline;
  uint32_t m_vertexCount = 0;
  uint32_t m_instanceCount = 0;
};

class OpenGLCopyImageToImage final : public IOpenGLCommand<OpenGLCopyImageToImage> {
 public:
  OpenGLCopyImageToImage() = default;
  OpenGLCopyImageToImage(const OpenGLCopyImageToImage& command)
      : m_srcTexture(command.m_srcTexture), m_dstTexture(command.m_dstTexture) {}

  virtual ~OpenGLCopyImageToImage() = default;

 public:
  void
  SetSrcImage(const std::shared_ptr<Texture>& srcTexture) {
    m_srcTexture = std::static_pointer_cast<OpenGLTexture>(srcTexture);
  }

  void
  SetDstImage(const std::shared_ptr<Texture>& dstTexture) {
    m_dstTexture = std::static_pointer_cast<OpenGLTexture>(dstTexture);
  }

  void
  Execute() const {
    // auto srcTex = *static_cast<GLuint*>(m_srcTexture->GetOriginHandle());
    auto srcTex = m_srcTexture->GetOpenGLTarget();
    // auto srcLevel = m_srcTexture->GetLevels();
    auto width = m_srcTexture->GetWidth();
    auto height = m_srcTexture->GetHeight();
    auto dstTex = m_dstTexture->GetOpenGLTarget();
    // auto dstLevel = m_dstTexture->GetLevels();
    glCopyImageSubData(srcTex, GL_TEXTURE_2D, 0, 0, 0, 0, dstTex, GL_TEXTURE_2D, 0, 0, 0, 0, width,
                       height, 1);
  }

  void
  OnEndRenderPass() const {}

 private:
  std::shared_ptr<OpenGLTexture> m_srcTexture = nullptr;
  std::shared_ptr<OpenGLTexture> m_dstTexture = nullptr;
};

using OpenGLCommandPoly_T =
    std::variant<OpenGLBeginRenderPass, OpenGLBindDescriptorSet, OpenGLEndRenderpass,
                 OpenGLBindPipeline, OpenGLBindIndexBuffer, OpenGLBindVertexBuffer, OpenGLDrawArray,
                 OpenGLDrawIndex, OpenGLCopyImageToImage>;

}  // namespace Marbas
