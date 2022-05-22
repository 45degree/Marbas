#pragma once

#include "Common/Common.hpp"
#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"

namespace Marbas {

class OpenGLFrameBuffer : public FrameBuffer {
 public:
  explicit OpenGLFrameBuffer(const FrameBufferInfo& info);

  ~OpenGLFrameBuffer() override;

 public:
  void
  Bind() const override;

  void
  UnBind() const override;

 private:
  GLuint frameBufferID;

  Vector<AttachmentDescription> m_attachmentInfos;
  Vector<std::shared_ptr<OpenGLTexture2D>> m_attachments;
};

}  // namespace Marbas
