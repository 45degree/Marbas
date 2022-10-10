#pragma once

#include "Common/Common.hpp"
#include "RHI/RHI.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

class ResourceManager;
class ResourceBase {
 protected:
  ResourceBase() = default;

 public:
  virtual ~ResourceBase() = default;

  virtual void
  LoadResource(RHIFactory* rhiFactory, const ResourceManager* ResourceManager) = 0;

 public:
  [[nodiscard]] bool
  IsLoad() const noexcept {
    return m_isLoad;
  }

 protected:
  bool m_isLoad = false;
};

}  // namespace Marbas
