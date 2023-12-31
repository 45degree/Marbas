#include "Common/MathCommon.hpp"

namespace Marbas::details {

class Light {
 public:
  Light() = default;
  virtual ~Light() = default;

 public:
  void
  SetPos(const glm::vec3& pos) {
    m_pos = pos;
  }

  glm::vec3
  GetPos() const {
    return m_pos;
  }

  void
  SetColor(const glm::vec3& color) {
    m_color = color;
  }

  glm::vec3
  GetColor() const {
    return m_color;
  }

  float
  GetEnergy() const {
    return m_energy;
  }

  void
  SetEnergy(float energy) {
    m_energy = energy;
  }

 protected:
  glm::vec3 m_pos;
  glm::vec3 m_color = glm::vec3(1, 1, 1);
  float m_energy = 1;
};

class PointLight final : public Light {
 public:
  PointLight() = default;
  ~PointLight() = default;

 public:
  float
  GetNearPlane() const {
    return m_near;
  }

  void
  SetNearPlane(float near) {
    m_near = near;
  }

  float
  GetFarPlane() const {
    return m_far;
  }

  void
  SetFarPlane(float far) {
    m_far = far;
  }

  void
  SetAspect(float aspect) {
    m_aspect = aspect;
  }

  glm::mat4
  GetProjectionMatrix() const {
    return glm::perspective(glm::radians(90.0f), 1.0f, m_near, m_far);
  }

  /**
   * @brief get view matrix to transform the world coordiante to the camera coordiante
   *
   * @param diretion the direction of the cubemap
   *        0 --- right
   *        1 --- left
   *        2 --- top
   *        3 --- bottom
   *        4 --- front
   *        5 --- back
   */
  glm::mat4
  GetViewMatrix(const int direction) const;

 private:
  float m_aspect = 1.0f;
  float m_near = 0.1f;
  float m_far = 1000.0f;
};

}  // namespace Marbas::details
