//
// Created by Matt Blair on 7/30/16.
//

#include "gl/Texture.hpp"
#include "gl/RenderState.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace stock {

static uint8_t defaultPixelData[4] = { 255, 0, 255, 255 };

Texture::Texture() {}

Texture::~Texture() {
    if (m_data != nullptr) {
        free(m_data);
    }
}

Texture::Texture(uint32_t width, uint32_t height, uint8_t* data, size_t size, Options options) :
        m_options(options), m_width(width), m_height(height), m_data(data), m_size(size) {}

bool Texture::loadImageFileData(Texture* texture, const uint8_t* data, size_t size, Options options) {

    if (texture->m_glHandle != 0) {
        // We can only load image data into a new texture that hasn't been uploaded to the GPU yet.
        return false;
    }

    // stbi_load_from_memory loads the image as starting from the top-left corner. This call flips the output such that
    // the data begins at the bottom-left corner, as required for OpenGL texture data.
    stbi_set_flip_vertically_on_load(1);

    uint8_t* pixels = nullptr;
    int width = 0, height = 0, components = 0;

    if (data != nullptr && size != 0) {
        pixels = stbi_load_from_memory(data, size, &width, &height, &components, 0);
    }

    if (pixels == nullptr) {
        // Image loading failed for some reason.
        return false;
    }

    switch (components) {
        case 1: options.pixelFormat = PixelFormat::ALPHA; break;
        case 2: options.pixelFormat = PixelFormat::LUMINANCE_ALPHA; break;
        case 3: options.pixelFormat = PixelFormat::RGB; break;
        case 4: options.pixelFormat = PixelFormat::RGBA; break;
        default: break;
    }

    texture->m_options = options;
    texture->m_width = width;
    texture->m_height = height;
    texture->m_data = pixels;
    texture->m_size = width * height * components;

    return true;

}

uint32_t Texture::width() {
    return m_width;
}

uint32_t Texture::height() {
    return m_height;
}

void Texture::prepare(RenderState& rs) {

    if (m_glHandle != 0 && rs.isValidGeneration(m_generation)) {
        return;
    }

    CHECK_GL(glGenTextures(1, &m_glHandle));

    rs.texture(m_target, m_glHandle);

    CHECK_GL(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(m_options.minFilter)));
    CHECK_GL(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(m_options.magFilter)));
    CHECK_GL(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, static_cast<GLenum>(m_options.wrapS)));
    CHECK_GL(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, static_cast<GLenum>(m_options.wrapT)));

    auto width = m_width;
    auto height = m_height;
    auto data = m_data;
    auto format = static_cast<GLenum>(m_options.pixelFormat);
    auto type = static_cast<GLenum>(m_options.pixelType);

    if (data == nullptr) {
        // This Texture doesn't have data assigned to it, so it will use a default solid color instead.
        data = &defaultPixelData[0];
        width = 1;
        height = 1;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }

    CHECK_GL(glTexImage2D(m_target, 0, format, width, height, 0, format, type, data));

    if (m_options.generateMipmaps) {
        CHECK_GL(glGenerateMipmap(m_target));
    }

    m_generation = rs.generation();

}

void Texture::bind(RenderState& rs) {

    prepare(rs);
    rs.texture(m_target, m_glHandle);

}

void Texture::dispose(RenderState& rs) {

    if (!rs.isValidGeneration(m_generation)) {
        return;
    }

    rs.textureUnset(m_target, m_glHandle);

    CHECK_GL(glDeleteTextures(1, &m_glHandle));

}

} // namespace stock
