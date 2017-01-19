//
// Created by Matt Blair on 1/14/17.
//
#pragma once

#include <glm/vec2.hpp>
#include <cassert>
#include <cstdint>

namespace stock {

class TileAddress {

public:

  TileAddress(uint32_t x, uint32_t y, uint8_t z) : x(x), y(y), z(z) {
    assert(z <= 32);
    assert(x < (1 << z));
    assert(y < (1 << z));
  }

  bool operator< (const TileAddress& other) const {
    return z < other.z || (z == other.z && (x < other.x || (x == other.x && y < other.y)));
  }

  bool operator==(const TileAddress& other) const {
    return z == other.z && x == other.x && y == other.y;
  }

  bool operator!=(const TileAddress& other) const {
    return z != other.z || x != other.x || y != other.y;
  }

  glm::dvec2 getOriginMercatorMeters() const;

  const uint32_t x;
  const uint32_t y;
  const uint8_t z;

};

} // namespace stock
