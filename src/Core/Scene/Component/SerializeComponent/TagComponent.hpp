#pragma once

namespace Marbas {

struct SunLightTag {
  template <typename Archive>
  void
  serialize(Archive&& archive) {
    archive();
  }
};

}  // namespace Marbas
