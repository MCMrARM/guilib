#pragma once

#include <vector>
#include <memory>
#include <guilib/color.h>

class ShaderProgram;

namespace guilib {

class Texture;

class MeshImpl {

public:

    int vertexArr = -1, colorArr = -1, texArr = -1;
    int vaoId = -1;
    ShaderProgram* program = nullptr;
    unsigned int vertexCount = 0;
    std::shared_ptr<Texture> texture;

    MeshImpl() { }
    MeshImpl(unsigned int vertexCount, int vertexArr, int colorArr, int texArr, int vaoId, ShaderProgram* program,
             std::shared_ptr<Texture> texture = std::shared_ptr<Texture>())
            : vertexCount(vertexCount), vertexArr(vertexArr), colorArr(colorArr), texArr(texArr), vaoId(vaoId),
              program(program), texture(texture) { }
    MeshImpl(MeshImpl&& i);
    ~MeshImpl();

    MeshImpl& operator=(MeshImpl&& mesh);

    void draw();

};

}