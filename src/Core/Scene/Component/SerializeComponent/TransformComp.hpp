#pragma once

#include <array>
#include <cereal/types/array.hpp>
#include <optional>

#include "Common/MathCommon.hpp"
#include "GLMSerialize.hpp"

namespace Marbas {

namespace details {
class SceneModelNodeSystem;
class SceneLightNodeSystem;
};  // namespace details

/**
 * @class TransformComp
 * @brief transform matrix component
 *
 * @warning if a node don't have the this componet, it means you can't add subnode.
 * @see Scene::CanAddSubNode
 *
 */
class TransformComp final {
 private:
  glm::mat4 m_globalTransform = glm::mat4(1.0);
  glm::mat4 m_localTransform = glm::mat4(1.0);
  bool m_isDirty = false;

 public:
  const glm::mat4&
  GetGlobalTransform() const {
    return m_globalTransform;
  }

  const glm::mat4&
  GetLocalTransform() const {
    return m_localTransform;
  }

  void
  SetGlobalTransform(const glm::mat4& globalTransform) {
    m_globalTransform = globalTransform;
    m_isDirty = false;
  }

  void
  SetLocalTransform(const glm::mat4& localTransform) {
    m_localTransform = localTransform;
    m_isDirty = true;
  }

  void
  SetLocalTransform(const glm::mat4& localTransform, const glm::mat4& parentGlobalTransform) {
    m_globalTransform = parentGlobalTransform * localTransform;
    m_localTransform = localTransform;
    m_isDirty = false;
  }

  bool
  IsDirty() const {
    return m_isDirty;
  }

  template <typename Archive>
  void
  serialize(Archive&& archive) {
    archive(m_globalTransform, m_localTransform);
  }
};

}  // namespace Marbas
