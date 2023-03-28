#pragma once

#include <glog/logging.h>

#include "Common/Common.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Renderer/SphericalHarmonic.hpp"

namespace Marbas::GI::PRTGI {

struct Surfel {
  enum class PrincipalDir : uint32_t {
    POSITIVE_X = 0,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z,
  };

  glm::vec3 m_pos;
  glm::vec3 m_normal;
  glm::vec3 m_albedo;

  Surfel() = default;
  Surfel(const glm::vec3& pos = glm::vec3(0, 0, 0), const glm::vec3& normal = glm::vec3(0, 0, 0),
         const glm::vec3& albedo = glm::vec3(0, 0, 0))
      : m_pos(pos), m_normal(normal), m_albedo(albedo) {}

  PrincipalDir
  GetPrincipalDir() const {
    if (std::abs(m_normal.x) > std::abs(m_normal.y) && std::abs(m_normal.x) > std::abs(m_normal.z)) {
      return m_normal.x > 0 ? PrincipalDir::POSITIVE_X : PrincipalDir::NEGATIVE_X;
    }
    if (std::abs(m_normal.y) > std::abs(m_normal.x) && std::abs(m_normal.y) > std::abs(m_normal.z)) {
      return m_normal.y > 0 ? PrincipalDir::POSITIVE_Y : PrincipalDir::NEGATIVE_Y;
    }
    if (std::abs(m_normal.z) > std::abs(m_normal.x) && std::abs(m_normal.z) > std::abs(m_normal.y)) {
      return m_normal.z > 0 ? PrincipalDir::POSITIVE_Z : PrincipalDir::NEGATIVE_Z;
    }
    DLOG_ASSERT(true) << "never enter this";
    return PrincipalDir::POSITIVE_X;
  }

  bool
  operator==(const Surfel& another) const {
    return GetPrincipalDir() == another.GetPrincipalDir() &&
           static_cast<int>(m_pos.x) == static_cast<int>(another.m_pos.x) &&
           static_cast<int>(m_pos.y) == static_cast<int>(another.m_pos.y) &&
           static_cast<int>(m_pos.z) == static_cast<int>(another.m_pos.z);
  }
};

struct Brick {
  Vector<SphericalHarmonic> transfer_weight;
  Vector<uint32_t> surfelIndex;
};

struct BrickFactor {
  double m_weight;
  uint32_t m_brickIndex;
};

struct LightProbe {
  glm::vec3 m_pos;
  Vector<BrickFactor> m_brickFactors;
};

};  // namespace Marbas::GI::PRTGI
