#pragma once

#include "texture_impl.h"

namespace guilib {

class Texture {

private:

    TextureImpl _impl;

public:

    Texture(TextureImpl&& impl) : _impl(std::move(impl)) { }
    Texture(Texture&& tex) : _impl(std::move(tex._impl)) { }

    Texture& operator=(Texture&& tex) {
        _impl = std::move(tex._impl);
        return *this;
    }

    /**
     * Return the internal texture implementation. You aren't really supposed to use it except for the engine wrapper
     * code.
     * @internal
     * @return the texture impl
     */
    TextureImpl& getImpl() { return _impl; }

};

}