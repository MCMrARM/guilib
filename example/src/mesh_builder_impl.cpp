#include "mesh_builder_impl.h"

#include <cstdio>
#include <epoxy/gl.h>
#include "shader.h"
#include "mesh_impl.h"
#include <guilib/texture.h>

using namespace guilib;

extern float projMatrix[16];

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
    if (hasUV)
        program = &ShaderProgram::defaultTexShader;
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

        glUniform1i(program->getSamplerUniform(), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->getImpl().getId());
        glBindSampler(0, tex->getImpl().getSamplerId());
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

    return MeshImpl(vertexCount, vertex, uv, color, vao, program, tex);
}