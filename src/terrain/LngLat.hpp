//
// Created by Matt Blair on 1/14/17.
//
#pragma once

#include <glm/vec2.hpp>

namespace stock {

class LngLat {

public:

  LngLat() {}
  LngLat(double longitude, double latitude) : longitude(longitude), latitude(latitude) {}
  LngLat(const LngLat& other) = default;
  LngLat(LngLat&& other) = default;
  LngLat& operator=(const LngLat& other) = default;
  LngLat& operator=(LngLat&& other) = default;

  bool operator==(const LngLat& other) {
    return longitude == other.longitude && latitude == other.latitude;
  }

  LngLat wrapToPositive() const;

  glm::dvec2 toMercatorMeters() const;

  static LngLat fromMercatorMeters(const glm::dvec2& meters);

  double longitude = 0.0;
  double latitude = 0.0;

};

} // namespace stock
