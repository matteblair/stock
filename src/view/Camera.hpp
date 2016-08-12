//
// Created by Matt Blair on 8/5/16.
//
#pragma once

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

    const glm::mat4& viewProjectionMatrix() const;

    const glm::mat3& normalMatrix() const;

    const glm::vec3& position() const;
    void setPosition(const glm::vec3& position);
    void setPosition(float x, float y, float z);

    const glm::vec3& upVector() const;
    void setUpVector(const glm::vec3& up);
    void setUpVector(float x, float y, float z);

    const glm::vec3& direction() const;
    void setDirection(const glm::vec3& direction);
    void setDirection(float x, float y, float z);

    void lookAt(const glm::vec3& target);
    void lookAt(float x, float y, float z);

    void translate(const glm::vec3& displacement);
    void translate(float x, float y, float z);

    void rotate(const glm::vec3& axis, float radians);
    void rotate(float axisX, float axisY, float axisZ, float radians);

    void orbit(const glm::vec3& target, const glm::vec3& axis, float radians);
    void orbit(float tX, float tY, float tZ, float aX, float aY, float aZ, float radians);

    void update();

    void apply(ShaderProgram& shader);

private:

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;
    glm::mat4 m_viewProjMatrix;
    glm::mat4 m_invViewProjMatrix;
    glm::mat3 m_normalMatrix;
    glm::mat3 m_invNormalMatrix;
    glm::vec3 m_position;
    glm::vec3 m_up = { 0.f, 0.f, 1.f };
    glm::vec3 m_direction { 0.f, 1.f, 0.f };
    Options m_options;
    float m_width = 1.f;
    float m_height = 1.f;
    bool m_dirty = true;

};

} // namespace stock
