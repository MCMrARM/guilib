#pragma once

#include "mesh_builder_impl.h"
#include "mesh.h"

namespace guilib {

class MeshBuilder {

private:

    MeshBuilderImpl _impl;

public:

    MeshBuilder() { }
    MeshBuilder(std::shared_ptr<Texture> tex) : _impl(tex) { }

    void color(GuiColor const& color) { _impl.color(color); }
    void vertex(float x, float y, float z) { _impl.vertex(x, y, z); }
    void vertexUV(float x, float y, float z, float u, float v) { _impl.vertexUV(x, y, z, u, v); }

    Mesh build() { return _impl.build(); }

};

}