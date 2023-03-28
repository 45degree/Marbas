#pragma once

#include "Common/Light.hpp"

namespace Marbas {

struct DirectionLightComponent {
  ParallelLight m_light;
};

struct PointLightComponent {
  PointLight m_light;
};

struct SunLightTag {};

}  // namespace Marbas
