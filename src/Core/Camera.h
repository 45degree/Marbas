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
        auto direction = glm::vec3(0, 0, 0);
        direction.x = std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw));
        direction.y = std::sin(glm::radians(m_pitch));
        direction.z = std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw));

        return glm::lookAt(m_position, m_position+direction, m_upDirection);
    }

    [[nodiscard]] glm::mat4 GetPerspective(float fov, float aspect) const noexcept {
        return glm::perspective(fov, aspect, m_near, m_far);
    }

    void SetPosition(const glm::vec3& newPos) {
        m_position = newPos;
    }

    void AddPitch(float pitch) noexcept {
        m_pitch += pitch;
        if(m_pitch > 89.0) {
            m_pitch = 89.0f;
        }
        else if(m_pitch < -89.0) {
            m_pitch = -89.0f;
        }
    }

    void AddYaw(float yaw) noexcept {
        m_yaw += yaw;
    }

private:
    float m_near = 0.1f;
    float m_far = 100.0f;
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    // glm::vec3 m_lookDirection = glm::vec3(0, 0, -1);
    glm::vec3 m_upDirection = glm::vec3(0, 1, 0);

    float m_pitch = 0.0f;
    float m_yaw = -180.0f;
};

#endif
