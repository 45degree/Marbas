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
  friend class Scene;
  friend class SceneSystem;

 private:
  glm::mat4 m_globalTransform = glm::mat4(1.0);

  /**
   * @brief the global transform matrix will be updated to this value in the next frame.
   * @see SceneSystem::UpdateTransformComp
   */
  std::optional<glm::mat4> m_updatedGlobalTransform = std::nullopt;

 public:
  const glm::mat4&
  GetGlobalTransform() const {
    if (m_updatedGlobalTransform.has_value()) {
      return *m_updatedGlobalTransform;
    }
    return m_globalTransform;
  }

  void
  SetGlobalTransform(const glm::mat4& transformComp) {
    m_updatedGlobalTransform = transformComp;
  }

  template <typename Archive>
  void
  serialize(Archive&& archive) {
    archive(m_globalTransform);
  }
};

}  // namespace Marbas
