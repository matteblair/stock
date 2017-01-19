//
// Created by Matt Blair on 8/5/16.
//

#include "view/Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

namespace stock {

Camera::Camera(float width, float height, Options options) : m_options(options), m_width(width), m_height(height) {}

float Camera::width() const {
  return m_width;
}

float Camera::height() const {
  return m_height;
}

void Camera::resize(float width, float height) {
  m_width = width;
  m_height = height;
}

float Camera::fieldOfView() const {
  return m_options.fov;
}

void Camera::setFieldOfView(float radians) {
  m_options.fov = radians;
}

Camera::Type Camera::projectionType() const {
  return m_options.type;
}

void Camera::setProjectionType(Type type) {
  m_options.type = type;
}

float Camera::nearDepth() const {
  return m_options.near;
}

void Camera::setNearDepth(float near) {
  m_options.near = near;
}

float Camera::farDepth() const {
  return m_options.far;
}

void Camera::setFarDepth(float far) {
  m_options.far = far;
}

glm::mat4 Camera::viewMatrix() const {
  auto matrix = m_transform.getWorldToLocalMatrix();

  // Change-of-basis into OpenGL view space with right=x, up=y, forward=-z.
  glm::mat4 R;
  R[0] = {1.f, 0.f, 0.f, 0.f};
  R[1] = {0.f, 0.f,-1.f, 0.f};
  R[2] = {0.f, 1.f, 0.f, 0.f};
  R[3] = {0.f, 0.f, 0.f, 1.f};
  auto view = R * matrix;
  return view;

}

glm::mat4 Camera::projectionMatrix() const {
  float hw = m_width * .5f;
  float hh = m_height * .5f;
  switch (m_options.type) {
  case Type::PERSPECTIVE:
    return glm::perspectiveFov(m_options.fov, m_width, m_height, m_options.near, m_options.far);
  case Type::ORTHOGRAPHIC:
    return glm::ortho(-hw, hw, -hh, hh, m_options.near, m_options.far);
  default:
    return glm::mat4(1);
  }
}

glm::mat4 Camera::viewProjectionMatrix() const {
  auto view = viewMatrix();
  auto proj = projectionMatrix();
  return proj * view;
}

glm::mat3 Camera::normalMatrix() const {
  auto view = viewMatrix();
  return glm::mat3(view);
}

const glm::vec3& Camera::upVector() const {
  return m_up;
}

void Camera::setUpVector(const glm::vec3& up) {
  m_up = up;
}

void Camera::lookAt(const glm::vec3& target) {
  m_transform.lookAt(target, m_up);
}

Camera::Ray Camera::getRayFromViewport(float x, float y) {
  Ray ray;
  glm::vec2 ndc;
  glm::vec2 nearHalfSize;
  glm::vec2 farHalfSize;

  // Re-scale the input window coordinates to normalized device coordinates.
  ndc.x = 2.f * (x / m_width) - 1.f;
  ndc.y = 1.f - 2.f * (y / m_height);

  // Calculate the half-size of the near and far planes.
  switch (m_options.type) {
  case Type::PERSPECTIVE:
    nearHalfSize.y = tanf(m_options.fov / 2.f) * m_options.near;
    nearHalfSize.x = nearHalfSize.y * (m_width / m_height);
    farHalfSize = nearHalfSize * (m_options.far / m_options.near);
    break;
  case Type::ORTHOGRAPHIC:
    nearHalfSize.y = .5f * m_height;
    nearHalfSize.x = .5f * m_width;
    farHalfSize = nearHalfSize;
    break;
  }

  glm::vec3 forward = m_transform.getDirection();
  glm::vec3 right = glm::normalize(glm::cross(forward, m_up));
  glm::vec3 up = glm::normalize(glm::cross(forward, right));

  glm::vec3 destination = m_options.far * forward + ndc.x * farHalfSize.x * right + ndc.y * farHalfSize.y * up;

  ray.origin = m_options.near * forward + ndc.x * nearHalfSize.x * right + ndc.y * nearHalfSize.y * up;
  ray.direction = destination - ray.origin;

  return ray;
}

} // namespace stock
