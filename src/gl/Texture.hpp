//
// Created by Matt Blair on 7/30/16.
//
#pragma once

#include "gl/GL.hpp"
#include <cstdint>

namespace stock {

class RenderState;

class Texture {

public:

    enum class PixelFormat : GLenum {
        ALPHA = GL_ALPHA,
        LUMINANCE = GL_LUMINANCE,
        LUMINANCE_ALPHA = GL_LUMINANCE_ALPHA,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
    };

    enum class PixelType : GLenum {
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    };

    enum class MinFilter : GLenum {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
    };

    enum class MagFilter : GLenum {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
    };

    enum class Wrap : GLenum {
        CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        REPEAT = GL_REPEAT,
    };

    struct Options {
        PixelFormat pixelFormat = PixelFormat::RGBA;
        PixelType pixelType = PixelType::UNSIGNED_BYTE;
        MinFilter minFilter = MinFilter::LINEAR;
        MagFilter magFilter = MagFilter::LINEAR;
        Wrap wrapS = Wrap::CLAMP_TO_EDGE;
        Wrap wrapT = Wrap::CLAMP_TO_EDGE;
        bool generateMipmaps = false;
    };

    // Create an empty Texture with default options.
    Texture();

    // Destroy the Texture and free the pixel data.
    ~Texture();

    // Create a Texture from a buffer of pixel data; the Texture takes ownership of the buffer.
    Texture(uint32_t width, uint32_t height, uint8_t* data, size_t size, Options options);

    // Load pixel data from the contents of an image file (JPG, PNG, GIF, BMP); this must be done before the first time
    // that prepare() or bind() is called on the Texture; the Texture does NOT take ownership of the buffer.
    static bool loadImageFileData(Texture* texture, const uint8_t* data, size_t size, Options options);

    // Get the image dimensions of the Texture.
    uint32_t width() const;
    uint32_t height() const;

    // Get the OpenGL handle of the Texture.
    GLuint glHandle() const;

    // Initialize OpenGL resources for the Texture, if needed.
    void prepare(RenderState& rs);

    // Bind the Texture to the current texture unit.
    void bind(RenderState& rs);

    // Delete any OpenGL resources for the Texture.
    void dispose(RenderState& rs);

protected:

    Options m_options;

    // Generation in which this texture's GL handle was created
    int m_generation = -1;

    // Image dimensions in pixels
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    // OpenGL texture identifiers
    GLuint m_glHandle = 0;
    GLenum m_target = GL_TEXTURE_2D;

    // Source pixel data
    static uint8_t defaultPixelData[];
    uint8_t* m_data = defaultPixelData;
    size_t m_size = 0;

};

} // namespace stock
