//
// Created by Matt Blair on 1/15/17.
//

#include "terrain/TileView.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>

namespace stock {

static constexpr double pixelsPerTile = 256.0;
static constexpr double earthRadius = 6378137.0; // meters
static constexpr double earthCircumference = 2.0 * M_PI * earthRadius;
static constexpr double earthHalfCircumference = M_PI * earthRadius;

void TileView::setPosition(LngLat position) {
  m_lngLat = position;
  m_isDirty = true;
}

void TileView::setZoom(double zoom) {
  m_zoom = zoom;
  m_isDirty = true;
}

void TileView::update() {

  if (!m_isDirty) {
    return;
  }
  // Update visible tile list.
  m_visibleTiles.clear();

  // Calculate effective altitude of viewpoint.
  // Using a reference field-of-view of 90 degrees, we can say that the camera altitude equals half of the vertical size
  // of the viewed area in world-space.
  double metersPerTile = earthCircumference * exp2(-m_zoom);
  double metersPerPixel = metersPerTile / pixelsPerTile;
  double altitude = m_camera.height() * metersPerPixel / 2.0;

  m_position = glm::dvec3(m_lngLat.toMercatorMeters(), altitude);

  // For full implementation:
  // Find corners of visible trapezoid on ground plane.
  // Find axis-aligned bounding box of visible area.
  // Iterate over AABB in tile space to populate tile list.

  // For now:
  // Find tile in center of viewport.
  auto positionMeters = m_lngLat.toMercatorMeters();
  auto beginMeters = positionMeters - (glm::dvec2(m_camera.width() / 2., m_camera.height() / 2.) * metersPerPixel);
  auto endMeters = positionMeters + (glm::dvec2(m_camera.width() / 2., m_camera.height() / 2.) * metersPerPixel);
  auto beginTiles = beginMeters / metersPerTile;
  auto endTiles = endMeters / metersPerTile + 1.;
  for (double x = beginTiles.x; x < endTiles.x; x += 1.) {
    for (double y = beginTiles.y; y < endTiles.y; y += 1.) {
      m_visibleTiles.emplace_back(floor(x), ceil(y), floor(m_zoom));
    }
  }

  m_isDirty = false;
}

glm::mat4 TileView::getModelViewProjectionMatrix(const TileAddress& address) const {

  glm::dvec3 tileOriginMeters(address.getOriginMercatorMeters(), 0.);
  glm::vec3 translation(tileOriginMeters - m_position);

  float scale = static_cast<float>(address.getSizeMercatorMeters());
  glm::mat4 modelMatrix(scale);

  // Apply translation to model matrix. 'y' translation is negated
  // because Mercator meters are 'y-down' but our view space is 'y-up'.
  modelMatrix[3][0] =  translation[0];
  modelMatrix[3][1] = -translation[1];
  modelMatrix[3][2] =  translation[2];

  return m_camera.viewProjectionMatrix() * modelMatrix;
}

} // namespace stock
