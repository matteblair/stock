//
// Created by Matt Blair on 5/28/18.
//
#pragma once

#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"

namespace stock {

class Transform {

public:

  static constexpr glm::vec3 RIGHT{1.f, 0.f, 0.f};
  static constexpr glm::vec3 FORWARD{0.f, 1.f, 0.f};
  static constexpr glm::vec3 UP{0.f, 0.f, 1.f};

  Transform() = default;

  Transform(const Transform&) = default;

  const glm::quat& rotation() const { return m_rotation; }
  glm::quat& rotation() { return m_rotation; }

  const glm::vec3& position() const { return m_position; }
  glm::vec3& position() { return m_position; }

  const glm::vec3& scale() const { return m_scale; }
  glm::vec3& scale() { return m_scale; }

  glm::vec3 convertLocalPointToWorld(const glm::vec3& localPoint) const;

  glm::vec3 convertWorldPointToLocal(const glm::vec3& worldPoint) const;

  glm::vec3 convertLocalVectorToWorld(const glm::vec3& localVector) const;

  glm::vec3 convertWorldVectorToLocal(const glm::vec3& worldVector) const;

  glm::mat4 getLocalToWorldMatrix() const;

  glm::mat4 getWorldToLocalMatrix() const;

  glm::vec3 getDirection() const;

  void setDirection(const glm::vec3& direction);

  void lookAt(const glm::vec3& target, const glm::vec3& up = UP);

  void translate(const glm::vec3& displacement);

  void rotate(const glm::vec3& axis, float radians);

  void orbit(const glm::vec3& target, const glm::vec3& axis, float radians);

private:
  glm::quat m_rotation;
  glm::vec3 m_position;
  glm::vec3 m_scale = glm::vec3(1.f);
};

} // namespace stock
