#pragma once

namespace guilib {

class GuiNinepatchImage {

private:

    float u1 = 0.f, u2 = 1.f;
    float v1 = 0.f, v2 = 1.f;

public:

    void reset();

    bool hasTexture() const;

};

}