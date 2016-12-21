//
// Created by Matt Blair on 12/18/16.
//
#pragma once

#include "gl/Mesh.hpp"
#include "gl/ShaderProgram.hpp"
#include "gl/Texture.hpp"
#include "view/Camera.hpp"

namespace stock {

class TerrainModel {

public:

  struct Vertex {
    Vertex(float x, float y) : x(x), y(y) {}
    float x = 0.f, y = 0.f;
  };

  TerrainModel();

  void loadElevationTexture(std::vector<uint8_t> elevationImageData);
  void loadNormalTexture(std::vector<uint8_t> normalData);

  void generateMesh(uint32_t resolution);

  void render(RenderState& rs, const Camera& camera);

  void toggleGrid();
  void toggleHull();
  uint32_t increaseResolution();
  uint32_t decreaseResolution();

  void dispose(RenderState& rs);

protected:

  Mesh <Vertex> m_mesh;
  ShaderProgram m_shader;
  UniformLocation m_elevTexLocation;
  UniformLocation m_normalTexLocation;
  UniformLocation m_mvpLocation;
  UniformLocation m_tintLocation;
  Texture m_elevationTexture;
  Texture m_normalTexture;
  uint32_t m_resolution = 0;
  bool m_gridIsOn = false;
  bool m_hullIsOn = true;
};

} // namespace stock
