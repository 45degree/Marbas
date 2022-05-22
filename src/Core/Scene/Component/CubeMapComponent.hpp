#pragma once

#include <memory>

namespace Marbas {

struct CubeMapComponent_Impl;
struct CubeMapComponent {
  std::shared_ptr<CubeMapComponent_Impl> m_implData = nullptr;
};

}  // namespace Marbas
