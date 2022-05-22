#pragma once

#include <memory>

#include "RHI/Interface/Texture.hpp"

namespace Marbas {

enum class AttachmentType {
  Color,
  Depth,
  Present,
};

enum class AttachmentLoadOp {
  Clear,
  Ignore,
};

struct AttachmentDescription {
  TextureFormat format;
  AttachmentType type = AttachmentType::Color;
  AttachmentLoadOp loadOp = AttachmentLoadOp::Ignore;
};

struct RenderPassCreateInfo {
  Vector<AttachmentDescription> attachments;
};

class RenderPass {
 public:
  explicit RenderPass(const RenderPassCreateInfo& createInfo) : m_createInfo(createInfo) {}
  virtual ~RenderPass() = default;

  const Vector<AttachmentDescription>&
  GetRenderPassAttachmentInfo() const noexcept {
    return m_createInfo.attachments;
  }

 protected:
  RenderPassCreateInfo m_createInfo;
};

}  // namespace Marbas
