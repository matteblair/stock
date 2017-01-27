//
// Created by Matt Blair on 1/25/17.
//

#include "view/ViewOrbit.hpp"

namespace stock {

void ViewOrbit::drag(float x, float y, bool down) {
  if (!down) {
    m_isTracking = false;
    return;
  }
  float dx = 0.f, dy = 0.f;
  if (m_isTracking) {
    dx = x - m_cursor.x;
    dy = y - m_cursor.y;
  }
  m_isTracking = true;
  m_cursor.x = x;
  m_cursor.y = y;
  m_quaternion = glm::rotate(m_quaternion, dx, glm::vec3(0., 1., 0.));
  m_quaternion = glm::rotate(m_quaternion, dy, glm::vec3(1., 0., 0.));
}

void ViewOrbit::reset() {
  m_quaternion = glm::quat();
  m_isTracking = false;
}

glm::vec3 ViewOrbit::getDirection() const {
  return m_quaternion * glm::vec3(0.f, 0.f, -1.f);
}

} // namespace stock
