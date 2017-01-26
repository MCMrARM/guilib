#pragma once

#include <vector>
#include <memory>
#include <guilib/color.h>

class ShaderProgram;

namespace guilib {

class MeshImpl;
class Texture;

class MeshBuilderImpl {

private:
    std::vector<float> arrVertex;
    std::vector<float> arrUV;
    std::vector<float> arrColor;
    GuiColor currentColor;
    bool hasUV, hasColor;
    unsigned int vertexCount = 0;
    std::shared_ptr<Texture> tex;

public:

    MeshBuilderImpl() : hasUV(false), hasColor(true) { }

    MeshBuilderImpl(std::shared_ptr<Texture> tex) : hasUV(true), hasColor(false), tex(tex) { }

    void color(GuiColor const& color);
    void vertex(float x, float y, float z);
    void vertexUV(float x, float y, float z, float u, float v);

    MeshImpl build();

};

}