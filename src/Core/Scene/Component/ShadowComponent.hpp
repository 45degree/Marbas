#pragma once

#include "RHI/RHI.hpp"

namespace Marbas {

struct ShadowComponent_Impl {
  std::shared_ptr<DescriptorSet> descriptorSet = nullptr;
};

struct PointShadowComponent_Impl {
  std::shared_ptr<DescriptorSet> descriptorSet = nullptr;
};

struct ShadowComponent {
  std::shared_ptr<ShadowComponent_Impl> implData;
  std::shared_ptr<PointShadowComponent_Impl> pointImplData;
};

}  // namespace Marbas
