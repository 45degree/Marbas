#include "Common/Light.hpp"

#include <glog/logging.h>

namespace Marbas {

glm::mat4
PointLight::GetViewMatrix(const int direction) const {
  auto dirVec = glm::vec3(0, 0, 0);
  auto upVec = glm::vec3(0, 0, 0);
  DLOG_ASSERT(direction < 6 && direction >= 0) << "direction must >= 0 and < 6";

  switch (direction) {
    case 0:
      dirVec = glm::vec3(1.0, 0.0, 0.0);
      upVec = glm::vec3(0.0, -1.0, 0.0);
      break;
    case 1:
      dirVec = glm::vec3(-1.0, 0, 0);
      upVec = glm::vec3(0, -1.0, 0);
      break;
    case 2:
      dirVec = glm::vec3(0, 1.0, 0);
      upVec = glm::vec3(0, 0., 1.0);
      break;
    case 3:
      dirVec = glm::vec3(0, -1.0, 0);
      upVec = glm::vec3(0, 0.0, -1.0);
      break;
    case 4:
      dirVec = glm::vec3(0, 0, 1.0);
      upVec = glm::vec3(0, -1.0, 0);
      break;
    case 5:
      dirVec = glm::vec3(0, 0, -1.0);
      upVec = glm::vec3(0, -1.0, 0);
      break;
  }

  return glm::lookAt(m_pos, m_pos + dirVec, upVec);
}

}  // namespace Marbas
