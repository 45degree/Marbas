#pragma once

#include <memory>

#include "Common/Light.hpp"

namespace Marbas {

struct LightComponent {
  std::unique_ptr<Light> m_light;
};

}  // namespace Marbas
