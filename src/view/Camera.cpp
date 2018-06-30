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
//  auto view = glm::mat4_cast(m_transform.rotation());
//  return glm::translate(view, m_transform.position());

//  auto translation = glm::translate(position());
//  auto rotation = glm::mat4_cast(m_transform.rotation());
//  return rotation * translation;

  auto eye = position();
  auto center = eye + direction();
  return glm::lookAt(eye, center, m_up);
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

const glm::vec3& Camera::position() const {
  return m_transform.position();
}

void Camera::setPosition(const glm::vec3& position) {
  m_transform.position() = position;
}

void Camera::setPosition(float x, float y, float z) {
  setPosition(glm::vec3(x, y, z));
}

const glm::vec3& Camera::upVector() const {
  return m_up;
}

void Camera::setUpVector(const glm::vec3& up) {
  m_up = up;
}

void Camera::setUpVector(float x, float y, float z) {
  setUpVector(glm::vec3(x, y, z));
}

glm::vec3 Camera::direction() const {
  return m_transform.getDirection();
}

void Camera::setDirection(const glm::vec3& direction) {
  m_transform.setDirection(direction);
}

void Camera::setDirection(float x, float y, float z) {
  setDirection(glm::vec3(x, y, z));
}

void Camera::lookAt(const glm::vec3& target) {
  m_transform.lookAt(target);
}

void Camera::lookAt(float x, float y, float z) {
  lookAt(glm::vec3(x, y, z));
}

void Camera::translate(const glm::vec3& displacement) {
  setPosition(position() + displacement);
}

void Camera::translate(float x, float y, float z) {
  translate(glm::vec3(x, y, z));
}

void Camera::rotate(const glm::vec3& axis, float radians) {
  glm::rotate(m_transform.rotation(), radians, axis);
}

void Camera::rotate(float axisX, float axisY, float axisZ, float radians) {
  rotate(glm::vec3(axisX, axisY, axisZ), radians);
}

void Camera::orbit(const glm::vec3& target, const glm::vec3& axis, float radians) {
  m_transform.orbit(target, axis, radians);
}

void Camera::orbit(float tX, float tY, float tZ, float aX, float aY, float aZ, float radians) {
  orbit(glm::vec3(tX, tY, tZ), glm::vec3(aX, aY, aZ), radians);
}

void Camera::apply(ShaderProgram& shader) {
  // TODO
}

} // namespace stock
