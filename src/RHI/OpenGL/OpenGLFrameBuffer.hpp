#pragma once

#include "Common/Common.hpp"
#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"
#include "RHI/OpenGL/OpenGLTexture.hpp"

namespace Marbas {

class OpenGLFrameBuffer final : public FrameBuffer, public IOpenGLTaget {
 public:
  explicit OpenGLFrameBuffer(const FrameBufferInfo& info);

  ~OpenGLFrameBuffer() override;

 public:
  void
  Bind() const override;

  void
  UnBind() const override;

  GLuint
  GetOpenGLTarget() const override {
    return frameBufferID;
  }

 private:
  GLuint frameBufferID;

  Vector<AttachmentDescription> m_attachmentInfos;
};

}  // namespace Marbas
