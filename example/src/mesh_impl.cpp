#include "mesh_impl.h"

#include <cstdio>
#include <epoxy/gl.h>
#include "shader.h"
#include <guilib/texture.h>

using namespace guilib;

extern float projMatrix[16];

MeshImpl::~MeshImpl() {
    if (vaoId != -1)
        glDeleteVertexArrays(1, (GLuint*) &vaoId);
    if (vertexArr != -1)
        glDeleteBuffers(1, (GLuint*) &vertexArr);
    if (texArr != -1)
        glDeleteBuffers(1, (GLuint*) &texArr);
    if (colorArr != -1)
        glDeleteBuffers(1, (GLuint*) &colorArr);
}

MeshImpl::MeshImpl(MeshImpl&& i) : vertexCount(i.vertexCount), colorArr(i.colorArr), texArr(i.texArr), vaoId(i.vaoId),
                                   program(i.program), texture(i.texture) {
    i.vaoId = i.vertexArr = i.colorArr = i.texArr = -1;
    i.program = nullptr;
    i.texture.reset();
    i.vertexCount = 0;
}

MeshImpl& MeshImpl::operator=(MeshImpl&& mesh) {
    this->vaoId = mesh.vaoId;
    this->vertexArr = mesh.vertexArr;
    this->colorArr = mesh.colorArr;
    this->texArr = mesh.texArr;
    this->program = mesh.program;
    this->vertexCount = mesh.vertexCount;
    this->texture = mesh.texture;
    mesh.vaoId = mesh.vertexArr = mesh.colorArr = mesh.texArr = -1;
    mesh.program = nullptr;
    mesh.vertexCount = 0;
    mesh.texture.reset();
    return *this;
}

void MeshImpl::draw() {
    glUseProgram(program->getId());
    glBindVertexArray((GLuint) vaoId);

    if (texture) {
        glUniform1i(program->getSamplerUniform(), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->getImpl().getId());
        glBindSampler(0, texture->getImpl().getSamplerId());
    }

    glUniformMatrix4fv(ShaderProgram::defaultColorShader.getProjMatrixUniform(), 1, GL_FALSE, projMatrix);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}