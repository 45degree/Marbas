#pragma once

#include "Common/Camera.hpp"
#include "Common/Common.hpp"
#include "Common/MathCommon.hpp"
#include "glog/logging.h"

/**
 * @brief this camera always looks at a fixed point, and can rotate around it
 */
namespace Marbas {

class EditorCamera final : public Camera {
 public:
  EditorCamera() {
    auto pos = glm::normalize(glm::vec3(-1, 1, 1)) * m_distance;
    m_viewMatrix = glm::lookAt(pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  }
  ~EditorCamera() = default;

 public:
  [[nodiscard]] glm::mat4
  GetViewMatrix() const noexcept override {
    return m_viewMatrix;
  }

  [[nodiscard]] glm::vec3
  GetPosition() const noexcept override {
    return glm::vec3(glm::column(glm::inverse(m_viewMatrix), 3));
  }

  [[nodiscard]] float
  GetDistance() const noexcept {
    return m_distance;
  }

  [[nodiscard]] glm::mat4
  GetProjectionMatrix() const noexcept override {
    return glm::perspective(glm::radians(fov), m_aspect, m_near, m_far);
  }

  glm::vec3
  GetUpVector() const noexcept override {
    return glm::normalize(glm::vec3(glm::column(glm::inverse(m_viewMatrix), 1)));
  }

  glm::vec3
  GetRightVector() const noexcept override {
    return glm::normalize(glm::vec3(glm::column(glm::inverse(m_viewMatrix), 0)));
  }

  float
  GetNear() const noexcept override {
    return m_near;
  }

  float
  GetFar() const noexcept override {
    return m_far;
  }

  glm::vec3
  GetLookAtVector() const noexcept {
    return -glm::normalize(glm::vec3(glm::column(glm::inverse(m_viewMatrix), 2)));
  }

  void
  SetViewMatrix(const glm::mat4& viewMatrix) {
    m_viewMatrix = viewMatrix;
  }

  void
  SetAspect(float aspect) noexcept {
    m_aspect = aspect;
  }

  glm::vec3
  GetLookPosition() {
    return GetPosition() + GetLookAtVector() * m_distance;
  }

  void
  AddPitch(float pitch) noexcept {
    auto lookVec = -GetLookAtVector() * m_distance;
    float curPitch = glm::degrees(std::asin(lookVec.y / glm::length(lookVec)));
    if (curPitch + pitch > 89.0 || curPitch + pitch < -89.0) return;

    auto inverseMatrix = glm::inverse(m_viewMatrix);

    auto rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(-pitch), GetRightVector());
    auto pos = GetLookPosition();
    auto transMatrix1 = glm::translate(glm::mat4(1.0), -pos);
    auto transMatrix2 = glm::translate(glm::mat4(1.0), pos);
    m_viewMatrix = glm::inverse(transMatrix2 * rotateMatrix * transMatrix1 * inverseMatrix);
  }

  void
  AddYaw(float yaw) noexcept {
    auto inverseMatrix = glm::inverse(m_viewMatrix);
    auto rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(yaw), glm::vec3(0, 1, 0));
    auto transMatrix1 = glm::translate(glm::mat4(1.0), -GetLookPosition());
    auto transMatrix2 = glm::translate(glm::mat4(1.0), GetLookPosition());
    m_viewMatrix = glm::inverse(transMatrix2 * rotateMatrix * transMatrix1 * inverseMatrix);
  }

  void
  MoveFixPoint(float xOffset, float yOffset, float zOffset) noexcept {
    auto transMatrix = glm::translate(glm::mat4(1.0), glm::vec3(xOffset, yOffset, zOffset));
    m_viewMatrix = glm::inverse(transMatrix * glm::inverse(m_viewMatrix));
  }

  void
  MovePosition(const glm::vec3& pos) {
    auto currentPos = GetLookPosition();
    auto transMatrix = glm::translate(glm::mat4(1.0), pos - currentPos);
    m_viewMatrix = glm::inverse(transMatrix * glm::inverse(m_viewMatrix));
  }

  void
  AddFov(float fovOffset) noexcept {
    if (fov >= 1.0f && fov <= 45.0f) {
      fov -= fovOffset;
    }
    if (fov <= 1.0f) {
      fov = 1.0f;
    }
    if (fov >= 45.0f) {
      fov = 45.0f;
    }
  }

  void
  AddDistance(float distanceDiff) {
    if (m_distance + distanceDiff > MaxDistance || m_distance + distanceDiff < MinDistance) return;
    auto transMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, distanceDiff));
    m_distance += distanceDiff;
    m_viewMatrix = glm::inverse(glm::inverse(m_viewMatrix) * transMatrix);
  }

 private:
  float m_near = 0.1f;
  float m_far = MaxDistance;
  float fov = 45.0f;
  float m_aspect = 800.f / 600.f;

  float m_distance = 50.0f;
  glm::mat4 m_viewMatrix;

  constexpr static float MaxDistance = 10000.f;
  constexpr static float MinDistance = 1.f;
};

}  // namespace Marbas
