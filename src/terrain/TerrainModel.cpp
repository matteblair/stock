//
// Created by Matt Blair on 12/18/16.
//

#include "TerrainModel.hpp"

namespace stock {

const static std::string vs_src = R"SHADER_END(
attribute vec2 a_position;
varying vec2 v_uv;
uniform mat4 u_mvp;
uniform sampler2D u_elevationTex;

void main() {
    vec4 encoded = texture2D(u_elevationTex, a_position);
    float elevation = (encoded.r * 256. * 256. + encoded.g * 256. + encoded.b) - 32768.;
    vec4 position = vec4(a_position, elevation / 19568., 1.);
    v_uv = a_position;
    gl_Position = u_mvp * position;
}
)SHADER_END";

const static std::string fs_src = R"SHADER_END(
varying vec2 v_uv;
uniform vec4 u_tint;
uniform sampler2D u_normalTex;
const vec3 lightDirection = vec3(.5, .5, -.5);

void main() {
    vec4 normal = texture2D(u_normalTex, v_uv);
    float brightness = .5 + dot(normal.xyz, lightDirection);
    gl_FragColor = u_tint * vec4(vec3(brightness), 1.);
}
)SHADER_END";

#define USE_TRIANGLE_STRIP 1

TerrainModel::TerrainModel()
    : m_shader(fs_src, vs_src),
      m_elevTexLocation("u_elevationTex"),
      m_normalTexLocation("u_normalTex"),
      m_mvpLocation("u_mvp"),
      m_tintLocation("u_tint") {
  m_mesh.setVertexLayout(VertexLayout({ VertexAttribute("a_position", 2, GL_UNSIGNED_BYTE, GL_TRUE) }));
}

void TerrainModel::loadElevationTexture(std::vector<uint8_t> data) {
  m_elevationTexture = Texture(Pixmap(data), Texture::Options());
}

void TerrainModel::loadNormalTexture(std::vector<uint8_t> data) {
  m_normalTexture = Texture(Pixmap(data), Texture::Options());
}

void TerrainModel::generateMesh(uint32_t resolution) {

  m_resolution = resolution;

  m_mesh.reset();

  #if USE_TRIANGLE_STRIP
  for (uint32_t col = 0; col < resolution; col++) {
    uint8_t y0 = static_cast<uint8_t>(col * 255 / resolution);
    uint8_t y1 = static_cast<uint8_t>((col + 1) * 255 / resolution);
    uint8_t y2 = static_cast<uint8_t>((col + 2) * 255 / resolution);
    uint8_t x = 0;
    for (uint32_t row = 0; row < resolution; row++) {
      x = static_cast<uint8_t>(row * 255 / resolution);
      m_mesh.vertices.push_back({x, y1});
      m_mesh.vertices.push_back({x, y0});
    }
    m_mesh.vertices.push_back({x, y0});
    m_mesh.vertices.push_back({0, y2});
  }
  #else
  uint16_t index = 0;
  for (uint32_t col = 0; col < resolution; col++) {
    coord_t y = static_cast<coord_t>(col * 255 / resolution);
    for (uint32_t row = 0; row < resolution; row++) {
      coord_t x = static_cast<coord_t>(row * 255 / resolution);
      m_mesh.vertices.push_back({x, y});

      if (row < resolution - 1 && col < resolution - 1) {
        m_mesh.indices.push_back(index);
        m_mesh.indices.push_back(index + 1);
        m_mesh.indices.push_back(index + resolution);

        m_mesh.indices.push_back(index + 1);
        m_mesh.indices.push_back(index + resolution + 1);
        m_mesh.indices.push_back(index + resolution);
      }

      index++;
    }
  }
  #endif
}

void TerrainModel::render(RenderState &rs, const Camera& camera) {

  m_elevationTexture.bind(rs, 0);
  m_shader.setUniformi(rs, m_elevTexLocation, 0);

  m_normalTexture.bind(rs, 1);
  m_shader.setUniformi(rs, m_normalTexLocation, 1);

  m_shader.setUniformMatrix4f(rs, m_mvpLocation, camera.viewProjectionMatrix());

  if (m_hullIsOn) {
    m_shader.setUniformf(rs, m_tintLocation, glm::vec4(1.f, 1.f, 1.f, 1.f));
    #if USE_TRIANGLE_STRIP
    m_mesh.setDrawMode(GL_TRIANGLE_STRIP);
    #else
    m_mesh.setDrawMode(GL_TRIANGLES);
    #endif
    m_mesh.draw(rs, m_shader);
  }

  if (m_gridIsOn) {
    m_shader.setUniformf(rs, m_tintLocation, glm::vec4(1.f, .2f, .2f, 1.f));
    #if USE_TRIANGLE_STRIP
    m_mesh.setDrawMode(GL_LINE_STRIP);
    #else
    m_mesh.setDrawMode(GL_LINES);
    #endif
    m_mesh.draw(rs, m_shader);
  }

}

void TerrainModel::toggleGrid() {
  m_gridIsOn = !m_gridIsOn;
}

void TerrainModel::toggleHull() {
  m_hullIsOn = !m_hullIsOn;
}

uint32_t TerrainModel::increaseResolution() {
  generateMesh(m_resolution * 2);
  return m_resolution;
}

uint32_t TerrainModel::decreaseResolution() {
  generateMesh(m_resolution / 2);
  return m_resolution;
}

void TerrainModel::dispose(RenderState& rs) {
  m_mesh.dispose(rs);
  m_shader.dispose(rs);
  m_elevationTexture.dispose(rs);
  m_normalTexture.dispose(rs);
}

} // namespace stock