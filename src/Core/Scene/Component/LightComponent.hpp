#pragma once

#include <memory>

#include "Common/Light.hpp"

namespace Marbas {

struct ParallelLightComponent {
  ParallelLight m_light;
};

struct PointLightComponent {
  PointLight m_light;
};

}  // namespace Marbas
