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

bool rayIntersectionWithGroundPlane(const Camera::Ray& ray, glm::vec2& result, float altitude) {
  if (ray.direction.z >= 0) {
    return false;
  }
  auto d = (ray.origin.z - altitude) / ray.direction.z;
  result.x = ray.origin.x + d * ray.direction.x;
  result.y = ray.origin.y + d * ray.direction.y;
  return true;
}

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

  // Calculate corners of view frustum intersection with ground plane.
  glm::vec2 bottomLeft, bottomRight, topLeft, topRight;

  rayIntersectionWithGroundPlane(m_camera.getRayFromViewport(0.f, 0.f), topLeft, altitude);
  rayIntersectionWithGroundPlane(m_camera.getRayFromViewport(m_camera.width(), 0.f), topRight, altitude);
  rayIntersectionWithGroundPlane(m_camera.getRayFromViewport(0.f, m_camera.height()), bottomLeft, altitude);
  rayIntersectionWithGroundPlane(m_camera.getRayFromViewport(m_camera.width(), m_camera.height()), bottomRight, altitude);

  auto beginMeters = positionMeters + glm::dvec2(topLeft);
  auto endMeters = positionMeters + glm::dvec2(bottomRight);
  auto beginTiles = beginMeters / metersPerTile;
  auto endTiles = endMeters / metersPerTile + 1.;
  auto tileX = beginTiles.x;
  while (tileX < endTiles.x) {
    auto tileY = beginTiles.y;
    while (tileY < endTiles.y) {
      m_visibleTiles.emplace_back(floor(tileX), ceil(tileY), floor(m_zoom));
      tileY += 1.;
    }
    tileX += 1.;
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
