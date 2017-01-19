//
// Created by Matt Blair on 1/14/17.
//

#include "terrain/TerrainData.hpp"
#include <cstdio>

namespace stock {

bool TerrainData::loadElevationData(const std::vector<uint8_t>& data) {
  m_elevationTexture = Texture(Pixmap(data), Texture::Options());
  return m_elevationTexture.width() > 0;
}

bool TerrainData::loadNormalData(const std::vector<uint8_t>& data) {
  m_normalTexture = Texture(Pixmap(data), Texture::Options());
  return m_normalTexture.width() > 0;
}

bool TerrainData::isLoaded() const {
  return m_elevationTexture.width() > 0 && m_normalTexture.width() > 0;
}

std::string TerrainData::populateUrlTemplate(const std::string& urlTemplate) {
  // Create a buffer of zeros with our maximum expected URL length.
  std::string buffer(256, 0);
  // Assuming the template has format specifiers for z, y, and x in that order, print the formatted string.
  int size = std::snprintf(&buffer[0], buffer.size(), urlTemplate.data(), m_address.z, m_address.x, m_address.y);
  // Trim the buffer to the size of the formatted string.
  buffer.resize(static_cast<size_t>(size));
  return buffer;
}

void TerrainData::dispose(RenderState &rs) {
  m_elevationTexture.dispose(rs);
  m_normalTexture.dispose(rs);
}

} // namespace stock
