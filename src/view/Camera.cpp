//
// Created by Matt Blair on 8/5/16.
//

#include "view/Camera.hpp"

namespace stock {

Camera::Camera() {}

const glm::vec3& Camera::position() {
    return m_position;
}

const glm::vec3& Camera::upVector() {
    return m_up;
}

const glm::vec3& Camera::direction() {
    return m_direction;
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    m_dirty = true;
}

void Camera::setPosition(float x, float y, float z) {
    setPosition(glm::vec3(x, y, z));
}

void Camera::setUpVector(const glm::vec3& up) {
    m_up = up;
    m_dirty = true;
}

void Camera::setUpVector(float x, float y, float z) {
    setUpVector(glm::vec3(x, y, z));
}

void Camera::setDirection(const glm::vec3& direction) {
    m_direction = direction;
    m_dirty = true;
}

void Camera::setDirection(float x, float y, float z) {
    setDirection(glm::vec3(x, y, z));
}

void Camera::lookAt(const glm::vec3& target) {
    // TODO
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
    // TODO
}

void Camera::rotate(float axisX, float axisY, float axisZ, float angle) {
    rotate(glm::vec3(axisX, axisY, axisZ), angle);
}

void Camera::orbit(const glm::vec3& target, const glm::vec3& axis, float angle) {
    // TODO
}

void Camera::orbit(float tX, float tY, float tZ, float aX, float aY, float aZ, float angle) {
    orbit(glm::vec3(tX, tY, tZ), glm::vec3(aX, aY, aZ), angle);
}

void Camera::update() {
    // TODO
}

void Camera::apply(ShaderProgram& shader) {
    // TODO
}

} // namespace stock
