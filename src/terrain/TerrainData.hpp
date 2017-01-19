//
// Created by Matt Blair on 1/14/17.
//
#pragma once

#include "gl/Texture.hpp"
#include "terrain/TileAddress.hpp"
#include <string>
#include <vector>

namespace stock {

class TerrainData {

public:

  TerrainData(TileAddress address) : m_address(address) {}

  const TileAddress& address() const { return m_address; }

  Texture& elevationTexture() { return m_elevationTexture; }
  Texture& normalTexture() { return m_normalTexture; }

  bool loadElevationData(const std::vector<uint8_t>& data);
  bool loadNormalData(const std::vector<uint8_t>& data);
  bool isLoaded() const;

  std::string populateUrlTemplate(const std::string& urlTemplate);

  void dispose(RenderState& rs);

protected:

  TileAddress m_address;
  Texture m_elevationTexture;
  Texture m_normalTexture;

};

} // namespace stock
