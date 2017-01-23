#pragma once

#include <vector>
#include <guilib/color.h>

class ShaderProgram;

namespace guilib {

class MeshImpl {

public:

    int vertexArr, colorArr, texArr;
    int vaoId;
    ShaderProgram* program;
    unsigned int vertexCount;

    MeshImpl(unsigned int vertexCount, int vertexArr, int colorArr, int texArr, int vaoId, ShaderProgram* program) :
            vertexCount(vertexCount), vertexArr(vertexArr), colorArr(colorArr), texArr(texArr), vaoId(vaoId),
            program(program) { }
    MeshImpl(MeshImpl&& i) : vertexCount(i.vertexCount), colorArr(i.colorArr), texArr(i.texArr), vaoId(i.vaoId),
                             program(i.program) {
        i.vaoId = i.vertexArr = i.colorArr = i.texArr = -1;
        i.program = nullptr;
        i.vertexCount = 0;
    }
    ~MeshImpl();

    void draw();

};

class MeshBuilderImpl {

private:
    std::vector<float> arrVertex;
    std::vector<float> arrUV;
    std::vector<float> arrColor;
    GuiColor currentColor;
    bool hasUV, hasColor;
    unsigned int vertexCount = 0;

public:

    MeshBuilderImpl(bool hasUV = false, bool hasColor = true) : hasUV(hasUV), hasColor(hasColor) { }

    void color(GuiColor const& color);
    void vertex(float x, float y, float z);
    void vertexUV(float x, float y, float z, float u, float v);

    MeshImpl build();

};

}