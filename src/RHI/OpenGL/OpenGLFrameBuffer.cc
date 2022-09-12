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
    auto attachment = std::static_pointer_cast<OpenGLImageView>(info.attachments[i]);
    auto texture = attachment->texture;
    auto textureId = texture->GetOpenGLTarget();
    auto textureType = texture->GetTextureType();
    auto textureFormat = texture->GetFormat();
    auto level = attachment->level;
    auto layer = attachment->layer;

    // check type
    if (texture->GetFormat() != m_attachmentInfos[i].format) {
      auto strMsg = "the texture's format is not equal the attachment info format";
      LOG(ERROR) << strMsg;
      throw std::runtime_error(strMsg);
    }

    // TODO:

    // if (textureType == TextureType::TEXTURE2D || textureType == TextureType::CUBEMAP) {
    if (textureFormat == TextureFormat::DEPTH) {
      glNamedFramebufferTexture(frameBufferID, GL_DEPTH_ATTACHMENT, textureId, 0);
      depthAttachmentCount++;
    } else {
      glNamedFramebufferTexture(frameBufferID, GL_COLOR_ATTACHMENT0 + colorAttachmentCount,
                                textureId, 0);
      colorAttachments.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentCount);
      colorAttachmentCount++;
    }
    //
    // } else {
    //   if (textureFormat == TextureFormat::DEPTH) {
    //     glNamedFramebufferTextureLayer(frameBufferID, GL_DEPTH_ATTACHMENT, textureId, level,
    //     layer); depthAttachmentCount++;
    //   } else {
    //     glNamedFramebufferTextureLayer(frameBufferID, GL_COLOR_ATTACHMENT0 +
    //     colorAttachmentCount,
    //                                    textureId, level, layer);
    //     colorAttachments.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentCount);
    //     colorAttachmentCount++;
    //   }
    // }
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
