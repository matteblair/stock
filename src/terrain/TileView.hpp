//
// Created by Matt Blair on 1/15/17.
//
#pragma once

#include "terrain/LngLat.hpp"
#include "terrain/TileAddress.hpp"
#include "view/Camera.hpp"
#include <vector>

namespace stock {

class TileView {

public:

  TileView() {}

  void setPosition(LngLat position);

  void setZoom(double zoom);

  void update();

  Camera& camera() { return m_camera; }

  const Camera& camera() const { return m_camera; }

  LngLat position() const { return m_lngLat; }

  double zoom() const { return m_zoom; }

  const std::vector<TileAddress>& visibleTiles() const { return m_visibleTiles; }

  glm::mat4 getModelViewProjectionMatrix(const TileAddress& address) const;

protected:

  std::vector<TileAddress> m_visibleTiles;
  Camera m_camera;
  LngLat m_lngLat;
  double m_zoom = 0.0;
  glm::dvec3 m_position;
  bool m_isDirty = true;

};

} // namespace stock
