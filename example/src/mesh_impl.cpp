#include <guilib/mesh_impl.h>

#include <cstdio>
#include <epoxy/gl.h>
#include "shader.h"

using namespace guilib;

extern float projMatrix[16];

MeshImpl::~MeshImpl() {/*
    if (vertexArrId != -1)
        glDeleteBuffers(1, (GLuint*) &vertexArrId);
    if (uvArrId != -1)
        glDeleteBuffers(1, (GLuint*) &uvArrId);
    if (colorArrId != -1)
        glDeleteBuffers(1, (GLuint*) &colorArrId);*/
}

void MeshImpl::draw() {
    glUseProgram(program->getId());
    glBindVertexArray((GLuint) vaoId);

    glUniformMatrix4fv(ShaderProgram::defaultColorShader.getProjMatrixUniform(), 1, GL_FALSE, projMatrix);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void MeshBuilderImpl::color(GuiColor const& color) {
    currentColor = color;
}

void MeshBuilderImpl::vertex(float x, float y, float z) {
    vertexCount++;
    arrVertex.push_back(x);
    arrVertex.push_back(y);
    arrVertex.push_back(z);
    if (hasColor) {
        arrColor.push_back(currentColor.r);
        arrColor.push_back(currentColor.g);
        arrColor.push_back(currentColor.b);
        arrColor.push_back(currentColor.a);
    }
}

void MeshBuilderImpl::vertexUV(float x, float y, float z, float u, float v) {
    vertex(x, y, z);
    arrUV.push_back(u);
    arrUV.push_back(v);
}

MeshImpl MeshBuilderImpl::build() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLint vertex, uv = -1, color = -1;

    ShaderProgram* program = &ShaderProgram::defaultColorShader;
    glUseProgram(program->getId());

    glGenBuffers(1, (GLuint*) &vertex);
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint) vertex);
    glBufferData(GL_ARRAY_BUFFER, arrVertex.size() * sizeof(float), arrVertex.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray((GLuint) program->getPositionAttr());
    glVertexAttribPointer((GLuint) program->getPositionAttr(), 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (hasUV) {
        glGenBuffers(1, (GLuint*) &uv);
        glBindBuffer(GL_ARRAY_BUFFER, (GLuint) uv);
        glBufferData(GL_ARRAY_BUFFER, arrUV.size() * sizeof(float), arrUV.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray((GLuint) program->getUVAttr());
        glBindBuffer(GL_ARRAY_BUFFER, (GLuint) color);
        glVertexAttribPointer((GLuint) program->getUVAttr(), 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
    if (hasColor) {
        glGenBuffers(1, (GLuint*) &color);
        glBindBuffer(GL_ARRAY_BUFFER, (GLuint) color);
        glBufferData(GL_ARRAY_BUFFER, arrColor.size() * sizeof(float), arrColor.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray((GLuint) program->getColorAttr());
        glVertexAttribPointer((GLuint) program->getColorAttr(), 4, GL_FLOAT, GL_FALSE, 0, 0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return MeshImpl(vertexCount, vertex, uv, color, vao, program);
}