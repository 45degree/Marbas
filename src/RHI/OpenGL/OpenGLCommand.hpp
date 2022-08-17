#pragma once

#include <glog/logging.h>

#include "OpenGLDescriptorSet.hpp"
#include "RHI/Interface/Command.hpp"
#include "RHI/OpenGL/OpenGLIndexBuffer.hpp"
#include "RHI/OpenGL/OpenGLPipeline.hpp"
#include "RHI/OpenGL/OpenGLRenderPass.hpp"
#include "RHI/OpenGL/OpenGLVertexBuffer.hpp"

namespace Marbas {

class OpenGLBindDescriptorSet final : public BindDescriptorSet {
 public:
  OpenGLBindDescriptorSet() = default;
  OpenGLBindDescriptorSet(const OpenGLBindDescriptorSet& command)
      : m_openGLDescriptor(command.m_openGLDescriptor) {}
  virtual ~OpenGLBindDescriptorSet() = default;

 public:
  void
  SetDescriptor(const std::shared_ptr<DescriptorSet>& descriptorSet) override {
    auto openglDescriptorSet = std::dynamic_pointer_cast<OpenGLDescriptorSet>(descriptorSet);
    DLOG_ASSERT(openglDescriptorSet != nullptr) << "can't set descriptorSet";
    m_openGLDescriptor = openglDescriptorSet;
  }

 protected:
  void
  Execute() const override {
    m_openGLDescriptor->Bind();
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLBindDescriptorSet>(*this);
  };

 private:
  std::shared_ptr<OpenGLDescriptorSet> m_openGLDescriptor = nullptr;
};

class OpenGLBindDynamicDescriptorSet final : public BindDynamicDescriptorSet {
 public:
  OpenGLBindDynamicDescriptorSet() = default;
  OpenGLBindDynamicDescriptorSet(const OpenGLBindDynamicDescriptorSet& command)
      : m_openGLDescriptor(command.m_openGLDescriptor) {}

  virtual ~OpenGLBindDynamicDescriptorSet() = default;

 public:
  void
  SetDescriptorSet(const std::shared_ptr<DynamicDescriptorSet>& descriptorSet) override {
    auto openglDescriptorSet = std::dynamic_pointer_cast<OpenGLDynamicDescriptorSet>(descriptorSet);
    DLOG_ASSERT(openglDescriptorSet != nullptr) << "can't set descriptorSet";
    m_openGLDescriptor = openglDescriptorSet;
  }

 protected:
  void
  Execute() const override {
    m_openGLDescriptor->Bind(m_offset, m_size);
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLBindDynamicDescriptorSet>(*this);
  }

 private:
  std::shared_ptr<OpenGLDynamicDescriptorSet> m_openGLDescriptor = nullptr;
};

class OpenGLBeginRenderPass final : public BeginRenderPass {
 public:
  OpenGLBeginRenderPass() = default;
  OpenGLBeginRenderPass(const OpenGLBeginRenderPass& command)
      : m_renderPass(command.m_renderPass), m_frameBuffer(command.m_frameBuffer) {}
  virtual ~OpenGLBeginRenderPass() = default;

 public:
  void
  SetRenderPass(const std::shared_ptr<RenderPass>& renderPass) override {
    m_renderPass = std::dynamic_pointer_cast<OpenGLRenderPass>(renderPass);

    LOG_IF(ERROR, m_renderPass == nullptr) << "render pass is not a opengl render pass";
  }

  void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) override {
    m_frameBuffer = std::dynamic_pointer_cast<OpenGLFrameBuffer>(frameBuffer);

    LOG_IF(ERROR, m_frameBuffer == nullptr) << "frame buffer is not a opengl frame buffer";
  }

  void
  SetClearColor(const std::array<float, 4>& clearColor) override {
    m_clearColor = clearColor;
  }

 protected:
  void
  Execute() const override {
    if (m_clearColor.has_value()) {
      const auto& value = m_clearColor.value();
      glClearColor(value[0], value[1], value[2], value[3]);
    }
    m_frameBuffer->Bind();
    m_renderPass->Bind();
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLBeginRenderPass>(*this);
  }

 private:
  std::shared_ptr<OpenGLRenderPass> m_renderPass = nullptr;
  std::shared_ptr<OpenGLFrameBuffer> m_frameBuffer = nullptr;
  std::optional<std::array<float, 4>> m_clearColor = std::nullopt;
};

class OpenGLEndRenderpass final : public EndRenderPass {
 public:
  OpenGLEndRenderpass() = default;
  OpenGLEndRenderpass(const OpenGLEndRenderpass& command) {}
  virtual ~OpenGLEndRenderpass() = default;

 public:
  void
  Execute() const override {
    if (m_frameBuffer != nullptr) {
      m_frameBuffer->UnBind();
    }
  }

  void
  SetRenderPass(const std::shared_ptr<RenderPass>& renderPass) override {
    m_renderPass = std::dynamic_pointer_cast<OpenGLRenderPass>(renderPass);

    LOG_IF(ERROR, m_renderPass == nullptr) << "render pass is not a opengl render pass";
  }

  void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) override {
    m_frameBuffer = std::dynamic_pointer_cast<OpenGLFrameBuffer>(frameBuffer);

    LOG_IF(ERROR, m_frameBuffer == nullptr) << "frame buffer is not a opengl frame buffer";
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLEndRenderpass>(*this);
  }

 private:
  std::shared_ptr<OpenGLRenderPass> m_renderPass = nullptr;
  std::shared_ptr<OpenGLFrameBuffer> m_frameBuffer = nullptr;
};

class OpenGLBindPipeline final : public BindPipeline {
 public:
  OpenGLBindPipeline() = default;
  OpenGLBindPipeline(const OpenGLBindPipeline& command) {}
  virtual ~OpenGLBindPipeline() = default;

 public:
  void
  SetPipeLine(const std::shared_ptr<GraphicsPipeLine>& graphicsPipeline) override {
    m_graphicsPipeline = std::dynamic_pointer_cast<OpenGLGraphicsPipeline>(graphicsPipeline);
    LOG_IF(ERROR, m_graphicsPipeline == nullptr)
        << "graphicsPipeline is not an opengl graphics pipeline";
  }

 private:
  void
  Execute() const override {
    if (m_graphicsPipeline != nullptr) {
      m_graphicsPipeline->Bind();
    }
  }

  void
  OnEndRenderPass() const override {
    if (m_graphicsPipeline != nullptr) {
      m_graphicsPipeline->UnBind();
    }
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLBindPipeline>(*this);
  }

 private:
  std::shared_ptr<OpenGLGraphicsPipeline> m_graphicsPipeline = nullptr;
};

class OpenGLBindVertexBuffer final : public BindVertexBuffer {
 public:
  OpenGLBindVertexBuffer() = default;
  OpenGLBindVertexBuffer(const OpenGLBindVertexBuffer& command)
      : m_vertexBuffer(command.m_vertexBuffer) {}
  virtual ~OpenGLBindVertexBuffer() = default;

 public:
  void
  SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override {
    m_vertexBuffer = std::dynamic_pointer_cast<OpenGLVertexBuffer>(vertexBuffer);
    LOG_IF(ERROR, m_vertexBuffer == nullptr) << "vertex buffer is not an opengl vertex buffer";
  }

  void
  Execute() const override {
    if (m_vertexBuffer != nullptr) {
      m_vertexBuffer->Bind();
    }
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLBindVertexBuffer>(*this);
  }

 private:
  std::shared_ptr<OpenGLVertexBuffer> m_vertexBuffer;
};

class OpenGLBindIndexBuffer final : public BindIndexBuffer {
 public:
  OpenGLBindIndexBuffer() = default;
  OpenGLBindIndexBuffer(const OpenGLBindIndexBuffer& command)
      : m_indexBuffer(command.m_indexBuffer) {}
  virtual ~OpenGLBindIndexBuffer() = default;

 public:
  void
  SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override {
    m_indexBuffer = std::dynamic_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
    LOG_IF(ERROR, m_indexBuffer == nullptr) << "index buffer is not an opengl index buffer";
  }

  void
  Execute() const override {
    m_indexBuffer->Bind();
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLBindIndexBuffer>(*this);
  }

 private:
  std::shared_ptr<OpenGLIndexBuffer> m_indexBuffer = nullptr;
};

class OpenGLDrawIndex final : public DrawIndex {
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
  SetIndexCount(int count) override {
    m_indexCount = count;
  }

  void
  SetInstanceCount(int count) override {
    m_instanceCount = count;
  }

  void
  Execute() const override {
    if (m_pipeline->GetVertexInputRate() == VertexInputRate::INSTANCE) {
      glDrawElementsInstanced(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0, m_instanceCount);
    } else {
      glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    }
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLDrawIndex>(*this);
  }

 private:
  std::shared_ptr<OpenGLGraphicsPipeline> m_pipeline;
  uint32_t m_indexCount = 0;
  uint32_t m_instanceCount = 0;
};

class OpenGLDrawArray final : public DrawArray {
 public:
  OpenGLDrawArray() = default;
  OpenGLDrawArray(const OpenGLDrawArray& command) : m_vertexCount(command.m_vertexCount) {}
  virtual ~OpenGLDrawArray() = default;

 public:
  void
  SetPipeline(const std::shared_ptr<OpenGLGraphicsPipeline>& pipeline) {
    m_pipeline = pipeline;
  }

  void
  SetVertexCount(int count) override {
    m_vertexCount = count;
  }

  void
  SetInstanceCount(int count) override {
    m_instanceCount = count;
  }

  void
  Execute() const override {
    if (m_pipeline->GetVertexInputRate() == VertexInputRate::INSTANCE) {
      glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, m_instanceCount);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLDrawArray>(*this);
  }

 private:
  std::shared_ptr<OpenGLGraphicsPipeline> m_pipeline;
  uint32_t m_vertexCount = 0;
  uint32_t m_instanceCount = 0;
};

class OpenGLCopyImageToImage final : public CopyImageToImage {
 public:
  OpenGLCopyImageToImage() = default;
  OpenGLCopyImageToImage(const OpenGLCopyImageToImage& command)
      : m_srcTexture(command.m_srcTexture), m_dstTexture(command.m_dstTexture) {}

  virtual ~OpenGLCopyImageToImage() = default;

 public:
  void
  SetSrcImage(const std::shared_ptr<Texture2D>& srcTexture) override {
    m_srcTexture = std::dynamic_pointer_cast<OpenGLTexture2D>(srcTexture);
  }

  void
  SetDstImage(const std::shared_ptr<Texture2D>& dstTexture) override {
    m_dstTexture = std::dynamic_pointer_cast<OpenGLTexture2D>(dstTexture);
  }

  void
  Execute() const override {
    auto srcTex = m_srcTexture->GetOpenGLTexture();
    auto srcLevel = m_srcTexture->GetLevel();
    auto width = m_srcTexture->GetWidth();
    auto height = m_srcTexture->GetHeight();
    auto depth = m_srcTexture->GetDepth();
    auto dstTex = m_dstTexture->GetOpenGLTexture();
    auto dstLevel = m_dstTexture->GetLevel();
    glCopyImageSubData(srcTex, GL_TEXTURE_2D, srcLevel - 1, 0, 0, 0, dstTex, GL_TEXTURE_2D,
                       dstLevel - 1, 0, 0, 0, width, height, 1);
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLCopyImageToImage>(*this);
  }

 private:
  std::shared_ptr<OpenGLTexture2D> m_srcTexture = nullptr;
  std::shared_ptr<OpenGLTexture2D> m_dstTexture = nullptr;
};

// TODO
class OpenGLCopyImageToFrameBuffer final : public CopyImageToFrameBuffer {
 public:
  OpenGLCopyImageToFrameBuffer() = default;
  OpenGLCopyImageToFrameBuffer(const OpenGLCopyImageToFrameBuffer& command)
      : m_texture(command.m_texture),
        m_frameBuffer(command.m_frameBuffer),
        m_type(command.m_type),
        m_attachmentIndex(command.m_attachmentIndex) {}

  virtual ~OpenGLCopyImageToFrameBuffer() = default;

 public:
  void
  SetImage(const std::shared_ptr<Texture2D>& texture) override {
    m_texture = std::dynamic_pointer_cast<OpenGLTexture2D>(texture);
  }

  void
  SetFrameBuffer(const std::shared_ptr<FrameBuffer>& frameBuffer) override {
    m_frameBuffer = std::dynamic_pointer_cast<OpenGLFrameBuffer>(frameBuffer);
  }

  void
  ChooseAttachment(AttachmentType attachmentType, uint32_t index) override {
    m_type = attachmentType;
    m_attachmentIndex = index;
  }

  void
  Execute() const override {
    if (m_type == AttachmentType::Depth) {
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
  }

  std::unique_ptr<ICommand>
  Clone() const override {
    return std::make_unique<OpenGLCopyImageToFrameBuffer>(*this);
  }

 private:
  std::shared_ptr<OpenGLTexture2D> m_texture;
  std::shared_ptr<OpenGLFrameBuffer> m_frameBuffer;
  AttachmentType m_type;
  uint32_t m_attachmentIndex = 0;
};

}  // namespace Marbas
