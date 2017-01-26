#pragma once

#include <epoxy/gl.h>

class Image;

namespace guilib {

class TextureImpl {

private:

    GLuint id;
    GLuint samplerId;
    bool hasId = false;

public:

    TextureImpl() { }
    TextureImpl(Image const& image);
    TextureImpl(TextureImpl&& impl);

    ~TextureImpl();

    TextureImpl& operator=(TextureImpl&& impl);

    GLuint getId() const { return id; }
    GLuint getSamplerId() const { return samplerId; }

};

}