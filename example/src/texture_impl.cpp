#include "texture_impl.h"

#include <epoxy/gl.h>
#include "image.h"

using namespace guilib;

TextureImpl::TextureImpl(Image const& image) {
    glGenTextures(1, &id);
    hasId = true;
    GLenum glInternalFormat = 0, glFormat = 0, glType = GL_UNSIGNED_BYTE;
    switch (image.getFormat()) {
        case ImageFormat::GRAY:
            glFormat = GL_ALPHA;
            glInternalFormat = GL_ALPHA;
            break;
        case ImageFormat::RGB:
            glFormat = GL_RGB;
            glInternalFormat = GL_RGB;
            break;
        case ImageFormat::RGBA:
            glFormat = GL_RGBA;
            glInternalFormat = GL_RGBA;
            break;
    }
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, image.getWidth(), image.getHeight(), 0, glFormat, glType,
                 image.getData().data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenSamplers(1, &samplerId);
    glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameterf(samplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
}

TextureImpl::TextureImpl(TextureImpl&& impl) : id(impl.id), hasId(impl.hasId) {
    impl.id = 0;
    impl.samplerId = 0;
    impl.hasId = false;
}

TextureImpl::~TextureImpl() {
    if (hasId) {
        glDeleteTextures(1, &id);
        glDeleteSamplers(1, &samplerId);
    }
}

TextureImpl& TextureImpl::operator=(TextureImpl&& impl) {
    id = impl.id;
    samplerId = impl.samplerId;
    hasId = impl.hasId;
    impl.id = 0;
    impl.samplerId = 0;
    impl.hasId = false;
    return *this;
}