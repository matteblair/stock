//
// Created by Matt Blair on 1/14/17.
//

#include "terrain/LngLat.hpp"
#include <cmath>

namespace stock {

static constexpr double pixelsPerTile = 256.0;
static constexpr double earthRadius = 6378137.0; // meters
static constexpr double earthCircumference = 2.0 * M_PI * earthRadius;
static constexpr double earthHalfCircumference = M_PI * earthRadius;

LngLat LngLat::wrapToPositive() const {
  LngLat lngLat;
  lngLat.longitude = longitude - floor(longitude / 360.0) * 360.0;
  lngLat.latitude = latitude - round(latitude / 180.0) * 180.0;
  return lngLat;
}

glm::dvec2 LngLat::toMercatorMeters() const {
  glm::dvec2 meters;
  meters.x = longitude * earthHalfCircumference / 180.0 + earthHalfCircumference;
  meters.y = -log(tan(0.25 * M_PI + latitude * M_PI / 360.0)) * earthRadius + earthHalfCircumference;
  return meters;
}

LngLat LngLat::fromMercatorMeters(const glm::dvec2 &meters) {
  // TODO: Implement me!
  return LngLat();
}

} // namespace stock
