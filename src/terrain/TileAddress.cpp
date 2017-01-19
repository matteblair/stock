//
// Created by Matt Blair on 1/14/17.
//

#include "terrain/TileAddress.hpp"
#include <cmath>

namespace stock {

static constexpr double earthRadius = 6378137.0; // meters
static constexpr double earthCircumference = 2.0 * M_PI * earthRadius;

glm::dvec2 TileAddress::getOriginMercatorMeters() const {
  double metersPerTile = earthCircumference * exp2(-static_cast<double>(z));
  return glm::dvec2(x * metersPerTile, y * metersPerTile);
}

} // namespace stock
