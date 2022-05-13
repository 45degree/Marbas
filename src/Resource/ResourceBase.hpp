#ifndef MARBAS_RESOURCE_RESOURCE_BASE_HPP
#define MARBAS_RESOURCE_RESOURCE_BASE_HPP

#include "Common.hpp"
#include "RHI/RHI.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

class ResourceBase {

 protected:
  ResourceBase() = default;

 public:
  virtual ~ResourceBase() = default;

  virtual void LoadResource(RHIFactory* rhiFactory) = 0;

 public:
  [[nodiscard]] Uid GetUid() const noexcept { return m_id; }

  [[nodiscard]] bool IsLoad() const noexcept { return m_isLoad; }

 protected:
  Uid m_id;
  bool m_isLoad = false;
};

}  // namespace Marbas

#endif
