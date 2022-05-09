#ifndef MARBARS_RHI_OPENGL_FRAMEBUFFER_H
#define MARBARS_RHI_OPENGL_FRAMEBUFFER_H

#include "Common.hpp"
#include "RHI/Interface/FrameBuffer.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

class OpenGLFrameBuffer : public FrameBuffer {
 public:
  explicit OpenGLFrameBuffer(const FrameBufferInfo& info);
  ~OpenGLFrameBuffer() override;

 public:
  void Create() override;
  void ReCreate() override;
  void Resize(int width, int height) override;

  void Bind() const override;
  void UnBind() const override;

  [[nodiscard]] const void* GetColorAttachTexture() const noexcept override {
    return reinterpret_cast<const void*>(colorAttachTexture);
  }

 private:
  GLuint frameBufferID;
  GLuint colorAttachTexture;
  GLuint depthAttachTexture;
};

}  // namespace Marbas

#endif
