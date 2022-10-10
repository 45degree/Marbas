#pragma once

#include "Common/Common.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

enum class EntityType {
  None,
  Mesh,
  Model,
  CubeMap,
  Light,
};

struct UniqueTagComponent {
  Uid uid;
  String tagName;
  EntityType type = EntityType::None;
};

struct MultiTagComponent {
  Vector<String> tagNames;
  EntityType type = EntityType::None;
};

struct EnableShadowTag {};

}  // namespace Marbas
