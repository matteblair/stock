//
// Created by Matt Blair on 8/5/16.
//
#pragma once

#include "transform/Transform.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace stock {

class ShaderProgram;

class Camera {

public:
  enum class Type : uint8_t {
    PERSPECTIVE,
    ORTHOGRAPHIC,
  };

  struct Options {
    float fov = 1.5708f;
    float near = 0.1f;
    float far = 1000.f;
    Type type = Type::PERSPECTIVE;
  };

  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
  };

  Camera(float width, float height, Options options);

  float width() const;
  float height() const;
  void resize(float width, float height);

  float fieldOfView() const;
  void setFieldOfView(float radians);

  Type projectionType() const;
  void setProjectionType(Type type);

  float nearDepth() const;
  void setNearDepth(float near);

  float farDepth() const;
  void setFarDepth(float far);

  glm::mat4 viewMatrix() const;

  glm::mat4 projectionMatrix() const;

  glm::mat4 viewProjectionMatrix() const;

  glm::mat3 normalMatrix() const;

  const Transform& transform() const { return m_transform; }
  Transform& transform() { return m_transform; }

  const glm::vec3& upVector() const;
  void setUpVector(const glm::vec3& up);

  void lookAt(const glm::vec3& target);

  Ray getRayFromViewport(float x, float y);

private:
  Transform m_transform;
  glm::vec3 m_up = Transform::UP;
  Options m_options;
  float m_width = 1.f;
  float m_height = 1.f;
};

} // namespace stock
