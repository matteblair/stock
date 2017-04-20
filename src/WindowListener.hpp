//
// Created by Matt Blair on 2/18/17.
//
#pragma once

namespace stock {

class WindowListener {
public:
  virtual void create() = 0;
  virtual void resize(uint32_t width, uint32_t height) = 0;
  virtual void render() = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;
  virtual void dispose() = 0;
};

static_assert(sizeof(WindowListener) == sizeof(nullptr), "WindowListener is a pure interface.");

} // namespace stock
