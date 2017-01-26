#pragma once

#include "mesh_impl.h"

namespace guilib {

class Mesh {

private:

    MeshImpl _impl;

public:

    Mesh() { }
    Mesh(MeshImpl&& impl) : _impl(std::move(impl)) { }
    Mesh(Mesh&& mesh) : _impl(std::move(mesh._impl)) { }

    void draw() { _impl.draw(); }

    Mesh& operator=(Mesh&& mesh) {
        _impl = std::move(mesh._impl);
        return *this;
    }

};

}