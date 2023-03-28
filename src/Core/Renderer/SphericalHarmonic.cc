#include "SphericalHarmonic.hpp"

#include <cmath>
#include <numbers>

namespace Marbas {

static double
SphericalHarmonic_L0M0(const glm::vec3& d) {
  return 0.5 * std::sqrt(1.0 / std::numbers::pi);
}

static double
SphericalHarmonic_L1Mn1(const glm::vec3& d) {
  return -0.5 * std::sqrt(3.0 / std::numbers::pi) * d.y;
}

static double
SphericalHarmonic_L1M0(const glm::vec3& d) {
  return 0.5 * std::sqrt(3.0 / std::numbers::pi) * d.z;
}

static double
SphericalHarmonic_L1Mp1(const glm::vec3& d) {
  return -0.5 * std::sqrt(3.0 / std::numbers::pi) * d.x;
}

static double
SphericalHarmonic_L2Mn2(const glm::vec3& d) {
  return 0.5 * std::sqrt(15.0 / std::numbers::pi) * d.x * d.y;
}

static double
SphericalHarmonic_L2Mn1(const glm::vec3& d) {
  return -0.5 * std::sqrt(15.0 / std::numbers::pi) * d.z * d.y;
}

static double
SphericalHarmonic_L2M0(const glm::vec3& d) {
  return 0.25 * std::sqrt(5.0 / std::numbers::pi) * (3 * d.z * d.z - 1);
}

static double
SphericalHarmonic_L2Mp1(const glm::vec3& d) {
  return -0.5 * std::sqrt(15.0 / std::numbers::pi) * d.z * d.x;
}

static double
SphericalHarmonic_L2Mp2(const glm::vec3& d) {
  return 0.25 * std::sqrt(15.0 / std::numbers::pi) * (d.x * d.x - d.y * d.y);
}

SphericalHarmonic::SphericalHarmonic(const glm::vec3& d) {
  m_cofe[0] = SphericalHarmonic_L0M0(d);
  m_cofe[1] = SphericalHarmonic_L1Mn1(d);
  m_cofe[2] = SphericalHarmonic_L1M0(d);
  m_cofe[3] = SphericalHarmonic_L1Mp1(d);
  m_cofe[4] = SphericalHarmonic_L2Mn2(d);
  m_cofe[5] = SphericalHarmonic_L2Mn1(d);
  m_cofe[6] = SphericalHarmonic_L2M0(d);
  m_cofe[7] = SphericalHarmonic_L2Mp1(d);
  m_cofe[8] = SphericalHarmonic_L2Mp2(d);
}

}  // namespace Marbas
