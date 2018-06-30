#include "Transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

namespace stock {

constexpr glm::vec3 Transform::UP;
constexpr glm::vec3 Transform::RIGHT;
constexpr glm::vec3 Transform::FORWARD;

glm::vec3 Transform::convertLocalPointToWorld(const glm::vec3& localPoint) const {
  return ((m_rotation * localPoint) * m_scale) + m_position;
}

glm::vec3 Transform::convertWorldPointToLocal(const glm::vec3& worldPoint) const {
  return glm::inverse(m_rotation) * ((worldPoint - m_position) / m_scale);
}

glm::vec3 Transform::convertLocalVectorToWorld(const glm::vec3& localVector) const {
  return (m_rotation * localVector) * m_scale;
}

glm::vec3 Transform::convertWorldVectorToLocal(const glm::vec3& worldVector) const {
  return glm::inverse(m_rotation) * (worldVector / m_scale);
}

glm::mat4 Transform::getLocalToWorldMatrix() const {
  glm::mat4 matrix(1);
  matrix = glm::rotate(matrix, glm::angle(m_rotation), glm::axis(m_rotation));
  matrix = glm::scale(matrix, m_scale);
  matrix = glm::translate(matrix, m_position);
  return matrix;
}

glm::mat4 Transform::getWorldToLocalMatrix() const {
  glm::mat4 matrix(1);
  matrix = glm::rotate(matrix, -glm::angle(m_rotation), glm::axis(m_rotation));
  matrix = glm::scale(matrix, 1.f / m_scale);
  matrix = glm::translate(matrix, -m_position);
  return matrix;
}

glm::vec3 Transform::getDirection() const {
  return m_rotation * FORWARD;
}

void Transform::setDirection(const glm::vec3& direction) {
  m_rotation = glm::quat(FORWARD, glm::normalize(direction));
}

void Transform::lookAt(const glm::vec3& target, const glm::vec3& up) {
  auto direction = glm::normalize(target - m_position);
  glm::mat3 matrix;
  matrix[0] = glm::normalize(glm::cross(direction, up));
  matrix[1] = direction;
  matrix[2] = glm::cross(matrix[0], direction);
  m_rotation = glm::quat_cast(matrix);
}

void Transform::translate(const glm::vec3& displacement) {
  m_position += displacement;
}

void Transform::rotate(const glm::vec3& axis, float radians) {
  m_rotation = glm::rotate(m_rotation, radians, axis);
}

void Transform::orbit(const glm::vec3& target, const glm::vec3& axis, float radians) {
  auto displacement = target - m_position;
  displacement = glm::rotate(displacement, radians, axis);
  m_position = target - displacement;
}

} // namespace stock
