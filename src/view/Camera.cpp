//
// Created by Matt Blair on 8/5/16.
//

#include "view/Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

namespace stock {

Camera::Camera() {}

float Camera::width() const {
    return m_width;
}

void Camera::setWidth(float width) {
    m_width = width;
    m_dirty = true;
}

float Camera::height() const {
    return m_height;
}

void Camera::setHeight(float height) {
    m_height = height;
    m_dirty = true;
}

float Camera::fieldOfView() const {
    return m_fov;
}

void Camera::setFieldOfView(float fov) {
    m_fov = fov;
    m_dirty = true;
}

Camera::Type Camera::projectionType() const {
    return m_type;
}

void Camera::setProjectionType(Type type) {
    m_type = type;
    m_dirty = true;
}

float Camera::nearDepth() const {
    return m_near;
}

void Camera::setNearDepth(float near) {
    m_near = near;
    m_dirty = true;
}

float Camera::farDepth() const {
    return m_far;
}

void Camera::setFarDepth(float far) {
    m_far = far;
    m_dirty = true;
}

const glm::mat4& Camera::viewProjectionMatrix() const {
    return m_viewProjMatrix;
}

const glm::mat3& Camera::normalMatrix() const {
    return m_normalMatrix;
}

const glm::vec3& Camera::position() const {
    return m_position;
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    m_dirty = true;
}

void Camera::setPosition(float x, float y, float z) {
    setPosition(glm::vec3(x, y, z));
}

const glm::vec3& Camera::upVector() const {
    return m_up;
}

void Camera::setUpVector(const glm::vec3& up) {
    m_up = up;
    m_dirty = true;
}

void Camera::setUpVector(float x, float y, float z) {
    setUpVector(glm::vec3(x, y, z));
}

const glm::vec3& Camera::direction() const {
    return m_direction;
}

void Camera::setDirection(const glm::vec3& direction) {
    m_direction = direction;
    m_dirty = true;
}

void Camera::setDirection(float x, float y, float z) {
    setDirection(glm::vec3(x, y, z));
}

void Camera::lookAt(const glm::vec3& target) {
    m_direction = glm::normalize(target - m_position);
    m_dirty = true;
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

void Camera::rotate(const glm::vec3& axis, float angle) {
    m_direction = glm::rotate(m_direction, angle, axis);
    m_dirty = true;
}

void Camera::rotate(float axisX, float axisY, float axisZ, float angle) {
    rotate(glm::vec3(axisX, axisY, axisZ), angle);
}

void Camera::orbit(const glm::vec3& target, const glm::vec3& axis, float angle) {
    auto displacement = target - m_position;
    displacement = glm::rotate(displacement, angle, axis);
    m_position = target - displacement;
    m_dirty = true;
}

void Camera::orbit(float tX, float tY, float tZ, float aX, float aY, float aZ, float angle) {
    orbit(glm::vec3(tX, tY, tZ), glm::vec3(aX, aY, aZ), angle);
}

void Camera::update() {

    if (!m_dirty) { return; }

    // Create view matrix.
    m_viewMatrix = glm::lookAt(m_position, (m_position + m_direction), m_up);

    // Create projection matrix.
    float hw = m_width * .5f;
    float hh = m_height * .5f;
    switch (m_type) {
        case Type::PERSPECTIVE:
            m_projMatrix = glm::perspectiveFov(m_fov, m_width, m_height, m_near, m_far);
            break;
        case Type::ORTHOGRAPHIC:
            m_projMatrix = glm::ortho(-hw, hw, -hh, hh, m_near, m_far);
            break;
    }

    // Create combined matrices.
    m_viewProjMatrix = m_projMatrix * m_viewMatrix;
    m_invViewProjMatrix = glm::inverse(m_viewProjMatrix);

    // Create normal matrix.
    m_normalMatrix = glm::mat3(m_viewMatrix);
    m_invNormalMatrix = glm::inverse(m_normalMatrix);

    // Reset dirty flag.
    m_dirty = false;

}

void Camera::apply(ShaderProgram& shader) {

    update();
    // TODO

}

} // namespace stock
