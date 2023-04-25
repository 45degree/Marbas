#pragma once

#include "RHIFactory.hpp"

namespace Marbas {

class GPUDataManager {
 public:
  static void
  SetUp(RHIFactory* rhiFactory);

  static void
  TearDown();
};

}  // namespace Marbas
