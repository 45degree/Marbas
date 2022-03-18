#ifndef MARBARS_CORE_CAMERA_H
#define MARBARS_CORE_CAMERA_H

#include "Common.h"

class Camera {
public:
    Camera() = default;
    ~Camera() = default;

public:
    [[nodiscard]] glm::vec3 GetPosition() const noexcept {
        return m_position;
    }

    [[nodiscard]] glm::mat4 GetViewMartix() const noexcept {
        return glm::lookAt(m_position, m_position+m_lookDirection, m_upDirection);
    }

    [[nodiscard]] glm::mat4 GetPerspective(float fov, float aspect) const noexcept {
        return glm::perspective(fov, aspect, m_near, m_far);
    }

    void SetPosition(const glm::vec3& newPos) {
        m_position = newPos;
    }

protected:
    float m_near = 0.1f;
    float m_far = 100.0f;
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::vec3 m_lookDirection = glm::vec3(0, 0, -1);
    glm::vec3 m_upDirection = glm::vec3(0, 1, 0);
};

#endif
