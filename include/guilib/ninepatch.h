#pragma once

#include <minecraft/client/renderer/TexturePtr.h>
#include <minecraft/client/renderer/Mesh.h>

namespace guilib {

class GuiNinepatchImage {

private:

    //mce::TexturePtr tex;
    float u1 = 0.f, u2 = 1.f;
    float v1 = 0.f, v2 = 1.f;
    //mce::Mesh mesh;

public:

    //void setTexture(mce::TexturePtr const& tex, float u1, float v1, float u2, float v2);

    void reset();

    bool hasTexture() const;

    mce::Mesh const& getMesh();

};

}