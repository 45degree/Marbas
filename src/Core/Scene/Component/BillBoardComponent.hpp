#pragma once

#include <optional>

#include "Common/MathCommon.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

class BillBoardComponent_Impl;

enum class BillBoardType {
  POINT_ROT_EYE,
  ASIX_ROT,
};

class BillBoardComponent {
 public:
  BillBoardType type = BillBoardType::POINT_ROT_EYE;
  glm::vec3 axis = glm::vec3(0.f, 1.0f, 0.f);
  glm::vec3 point = glm::vec3(0, 0, 0);
  glm::vec2 size = glm::vec2(1, 1);
  std::optional<Uid> textureResourceId;
  std::shared_ptr<BillBoardComponent_Impl> implData = nullptr;

#pragma pack(push, 1)

  struct UniformBufferBlockData {};

#pragma pack(pop)
};

}  // namespace Marbas
