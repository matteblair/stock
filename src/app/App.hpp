//
// Created by Matt Blair on 2/18/17.
//
#pragma once

#include "gl/Mesh.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"
#include "gl/ShaderUniform.hpp"
#include "view/Camera.hpp"
#include "WindowListener.hpp"

namespace stock {

class App : public WindowListener {

public:

  App();

  void create() override;
  void resize(uint32_t width, uint32_t height) override;
  void render() override;
  void pause() override;
  void resume() override;
  void dispose() override;

protected:

  struct Vertex {
    float x, y, z;
    unsigned int color;
  };

  Mesh<Vertex> mesh;
  ShaderProgram shader;
  UniformLocation mvpMatrixLocation;
  RenderState rs;
  Camera camera;

};

}
