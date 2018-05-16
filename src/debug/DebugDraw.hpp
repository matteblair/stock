//
// Created by Matt Blair on 4/3/18.
//
#pragma once

#include "gl/RenderState.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include <vector>

namespace stock {

namespace DebugDraw {

void cameraMatrix(const glm::mat4& matrix);

void point(RenderState& rs, const glm::vec3& position);

void linestring(RenderState& rs, const std::vector<glm::vec3>& positions);

} // namespace DebugDraw

} // namespace stock
