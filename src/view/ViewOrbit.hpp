//
// Created by Matt Blair on 1/25/17.
//
#pragma once
#include "glm/gtc/quaternion.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace stock {

class ViewOrbit {

public:

  ViewOrbit() {}

  void drag(float x, float y, bool down);

  void reset();

  glm::vec3 getDirection() const;

private:

  glm::quat m_quaternion;
  glm::vec2 m_cursor;
  bool m_isTracking = false;

};

} // namespace stock
