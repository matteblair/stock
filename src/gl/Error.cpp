//
// Created by Matt Blair on 4/29/16.
//
#include "gl/Error.hpp"

namespace stock {

const char* Error::getGlErrorString(GLenum error) {
  switch (error) {
  case GL_NO_ERROR: return "GL_NO_ERROR";
  case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
  case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
  default: return "unknown error";
  }
}

} // namespace stock
