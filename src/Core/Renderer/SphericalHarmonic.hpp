#pragma once

#include <array>

#include "Common/MathCommon.hpp"

namespace Marbas {

class SphericalHarmonic final {
 public:
  SphericalHarmonic() = default;
  SphericalHarmonic(const glm::vec3& d);
  ~SphericalHarmonic() = default;

  SphericalHarmonic&
  operator+=(const SphericalHarmonic& another) {
    for (int i = 0; i < m_cofe.size(); i++) {
      m_cofe[i] += another.m_cofe[i];
    }
    return *this;
  }

  SphericalHarmonic
  operator*(double m) {
    SphericalHarmonic res;
    for (int i = 0; i < m_cofe.size(); i++) {
      res.m_cofe[i] = m_cofe[i] * m;
    }

    return res;
  }

 private:
  std::array<double, 9> m_cofe;
};

}  // namespace Marbas
