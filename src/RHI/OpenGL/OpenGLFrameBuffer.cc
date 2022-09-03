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
    // check depth attachment count
    if (m_attachmentInfos[i].format == TextureFormat::DEPTH && depthAttachmentCount > 1) {
      auto strMsg = "the depth attachment should only have one";
      LOG(ERROR) << strMsg;
      throw std::runtime_error(strMsg);
    }
    auto attachment = info.attachments[i];

    // check type
    if (attachment->GetFormat() != m_attachmentInfos[i].format) {
      auto strMsg = "the texture's format is not equal the attachment info format";
      LOG(ERROR) << strMsg;
      throw std::runtime_error(strMsg);
    }

    // TODO:
    GLuint texture;
    if (attachment->GetTextureType() == TextureType::TEXTURE2D) {
      auto attachment2D = std::dynamic_pointer_cast<OpenGLTexture2D>(attachment);
      DLOG_ASSERT(attachment2D != nullptr)
          << FORMAT("the {}th attachment is not a opengl texture", i);
      texture = attachment2D->GetOpenGLTexture();
    } else if (info.attachments[i]->GetTextureType() == TextureType::CUBEMAP) {
      auto attachmentCubeMap = std::dynamic_pointer_cast<OpenGLTextureCubeMap>(attachment);
      DLOG_ASSERT(attachmentCubeMap != nullptr)
          << FORMAT("the {}th attachment is not a opengl cubemap texture", i);
      texture = attachmentCubeMap->GetOpenGLTexture();
    }

    // attach the attachment
    switch (attachment->GetFormat()) {
      case TextureFormat::DEPTH:
        glNamedFramebufferTexture(frameBufferID, GL_DEPTH_ATTACHMENT, texture, 0);
        depthAttachmentCount++;
        break;
      default:
        glNamedFramebufferTexture(frameBufferID, GL_COLOR_ATTACHMENT0 + colorAttachmentCount,
                                  texture, 0);
        colorAttachments.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentCount);
        colorAttachmentCount++;
        break;
    }

    m_attachments.push_back(std::move(attachment));
  }
  glDrawBuffers(colorAttachmentCount, colorAttachments.data());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    LOG(ERROR) << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";

  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

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
}

void
OpenGLFrameBuffer::UnBind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace Marbas
