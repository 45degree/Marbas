#include "RHI/OpenGL/OpenGLRenderPass.hpp"

namespace Marbas {

void
OpenGLRenderPass::Bind() {
  bool isClearColor = false;
  bool isClearDepth = false;
  const auto& m_attachmentInfos = m_createInfo.attachments;
  for (const auto& attachmentInfo : m_attachmentInfos) {
    if (attachmentInfo.loadOp == AttachmentLoadOp::Clear) {
      switch (attachmentInfo.type) {
        case AttachmentType::Depth:
          isClearDepth = true;
          break;
        case AttachmentType::Color:
          isClearColor = true;
          break;
        default:
          break;
      }
    }
  }

  if (isClearColor) glClear(GL_COLOR_BUFFER_BIT);
  if (isClearDepth) glClear(GL_DEPTH_BUFFER_BIT);
}

}  // namespace Marbas
