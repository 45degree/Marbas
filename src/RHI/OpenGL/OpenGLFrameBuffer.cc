#include "RHI/OpenGL/OpenGLFrameBuffer.hpp"

#include <glog/logging.h>

namespace Marbas {

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferInfo& info) : FrameBuffer(info) {
  // Create a default frame buffer
  if (info.renderPass == nullptr) {
    frameBufferID = 0;
    return;
  }

  // copy attachment infos
  m_attachmentInfos = info.renderPass->GetRenderPassAttachmentInfo();

  glGenFramebuffers(1, &frameBufferID);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

  int colorAttachmentCount = 0;
  int depthAttachmentCount = 0;
  Vector<GLuint> colorAttachments;
  for (int i = 0; i < m_attachmentInfos.size(); i++) {
    auto attachment = std::dynamic_pointer_cast<OpenGLTexture2D>(info.attachments[i]);
    if (attachment == nullptr) {
      auto strMsg = FORMAT("the {}th attachment is not a opengl texture", i);
      LOG(ERROR) << strMsg;
      throw std::runtime_error(strMsg);
    }

    // check type
    if (attachment->GetFormat() != m_attachmentInfos[i].format) {
      auto strMsg = "the texture's format is not equal the attachment info format";
      LOG(ERROR) << strMsg;
      throw std::runtime_error(strMsg);
    }

    // check depth attachment count
    if (m_attachmentInfos[i].format == TextureFormat::DEPTH && depthAttachmentCount > 1) {
      auto strMsg = "the depth attachment should only have one";
      LOG(ERROR) << strMsg;
      throw std::runtime_error(strMsg);
    }

    // attach the attachment
    auto texture = attachment->GetOpenGLTexture();
    switch (attachment->GetFormat()) {
      case TextureFormat::DEPTH:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
        depthAttachmentCount++;
        break;
      default:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentCount,
                               GL_TEXTURE_2D, texture, 0);
        colorAttachments.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentCount);
        colorAttachmentCount++;
        break;
    }

    m_attachments.push_back(std::move(attachment));
  }
  glDrawBuffers(colorAttachmentCount, colorAttachments.data());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    LOG(ERROR) << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";

  UnBind();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
  // if the frame buffer is not a default buffer, delete it
  if (frameBufferID) {
    glDeleteFramebuffers(1, &frameBufferID);
  }
}

void
OpenGLFrameBuffer::Bind() const {
  GLint currentBinding = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBinding);
  if (currentBinding == frameBufferID) return;

  glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

  auto error = glGetError();
  LOG_IF(ERROR, error) << FORMAT("can't bind framebuffer: {}, error code is {}", frameBufferID,
                                 error);
}

void
OpenGLFrameBuffer::UnBind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  auto error = glGetError();
  LOG_IF(ERROR, error) << FORMAT("can't unbind framebuffer: {}, error code is {}", frameBufferID,
                                 error);
}

}  // namespace Marbas
