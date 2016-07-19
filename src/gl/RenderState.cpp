//
// Created by Matt Blair on 4/2/16.
//
#include "gl/RenderState.hpp"
#include "gl/VertexLayout.hpp"

namespace stock {

GLuint RenderState::getTextureUnit(GLuint _unit) {
    return GL_TEXTURE0 + _unit;
}

void RenderState::bindVertexBuffer(GLuint _id) {
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, _id));
}

void RenderState::bindIndexBuffer(GLuint _id) {
    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id));
}

void RenderState::activeTextureUnit(GLuint _unit) {
    CHECK_GL(glActiveTexture(getTextureUnit(_unit)));
}

void RenderState::bindTexture(GLenum _target, GLuint _textureId) {
    CHECK_GL(glBindTexture(_target, _textureId));
}

void RenderState::configure() {
    m_textureUnit = -1;
    m_validGeneration++;
    attributeBindings = { 0 };

    blending.init(GL_FALSE);
    blendingFunc.init(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    culling.init(GL_TRUE);
    cullFace.init(GL_BACK);
    frontFace.init(GL_CCW);
    depthTest.init(GL_TRUE);
    depthWrite.init(GL_TRUE);

    CHECK_GL(glDisable(GL_STENCIL_TEST));
    CHECK_GL(glDepthFunc(GL_LEQUAL));
    // CHECK_GL(glClearDepthf(1.0));
    // CHECK_GL(glDepthRangef(0.0, 1.0));

    static size_t max = std::numeric_limits<size_t>::max();

    clearColor.init(0.0, 0.0, 0.0, 0.0);
    shaderProgram.init(max, false);
    vertexBuffer.init(max, false);
    indexBuffer.init(max, false);
    texture.init(GL_TEXTURE_2D, max, false);
    texture.init(GL_TEXTURE_CUBE_MAP, max, false);
    textureUnit.init(max, false);
}

bool RenderState::isValidGeneration(int _generation) {
    return _generation == m_validGeneration;
}

int RenderState::generation() {
    return m_validGeneration;
}

int RenderState::nextAvailableTextureUnit() {
    // TODO: Check if texture unit count exceeds GL_MAX_COMBINED_TEXTURE_UNITS
    // if (s_textureUnit + 1 > Hardware::maxCombinedTextureUnits) {
    //     LOGE("Too many combined texture units are being used");
    //     LOGE("GPU supports %d combined texture units", Hardware::maxCombinedTextureUnits);
    // }

    return ++m_textureUnit;
}

int RenderState::currentTextureUnit() {
    return m_textureUnit;
}

void RenderState::resetTextureUnit() {
    m_textureUnit = -1;
}

} // namespace stock

