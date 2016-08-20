//
// Created by Matt Blair on 4/29/16.
//
#pragma once

#include "gl/GL.hpp"
#include "io/Log.hpp"

namespace stock {

class Error {

public:
  static const char* getGlErrorString(GLenum error);
};

#if defined(NDEBUG)
#define CHECK_GL(X) X;
#else
#define CHECK_GL(X)                                                                                                    \
  X;                                                                                                                   \
  while (GLenum error = glGetError()) {                                                                                \
    Log::ef("GL error in %s:%d %s\n", __FILE__, __LINE__, Error::getGlErrorString(error));                             \
  }
#endif

} // namespace stock
