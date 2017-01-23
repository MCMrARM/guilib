#pragma once

#include <string>
#include <epoxy/gl.h>

class ShaderProgram {

private:

    GLuint id;
    bool hasId = false;
    GLint posAttr, colAttr, uvAttr;
    GLint projUni;

    static void compileShader(GLuint shader);

public:

    ShaderProgram() { }

    ShaderProgram(std::string const& vert, std::string const& frag);

    ShaderProgram(ShaderProgram&& prog);

    ~ShaderProgram();

    ShaderProgram& operator=(ShaderProgram&& prog);

    GLuint getId() const { return id; }

    GLint getPositionAttr() const { return posAttr; }

    GLint getColorAttr() const { return colAttr; }

    GLint getUVAttr() const { return uvAttr; }

    GLint getProjMatrixUniform() const { return projUni; }

    static ShaderProgram fromFiles(std::string vertexPath, std::string fragmentPath);

    static ShaderProgram defaultColorShader;

};