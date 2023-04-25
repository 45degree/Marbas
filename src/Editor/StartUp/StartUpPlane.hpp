#pragma once

#include <optional>

#include "Common/Common.hpp"
// #include "Common"

namespace Marbas {

class StartUpPlane {
 public:
  void
  Render();

  std::optional<Path> m_projectPath = std::nullopt;
};

}  // namespace Marbas
