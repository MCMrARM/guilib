#include "shader.h"

#include <sstream>
#include <fstream>
#include <stdexcept>

ShaderProgram ShaderProgram::defaultColorShader;
ShaderProgram ShaderProgram::defaultTexShader;

void ShaderProgram::compileShader(GLuint shader) {
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::string log;
            log.resize((size_t) logLength);
            glGetShaderInfoLog(shader, logLength, NULL, &log[0]);
            throw std::runtime_error("Failed to compile shader: " + log);
        }
    }
}

ShaderProgram::ShaderProgram(std::string const& vert, std::string const& frag) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    GLchar const* source = vert.data();
    glShaderSource(vs, 1, &source, NULL);
    source = frag.data();
    glShaderSource(fs, 1, &source, NULL);

    compileShader(vs);
    compileShader(fs);

    id = glCreateProgram();
    hasId = true;
    glAttachShader(id, vs);
    glAttachShader(id, fs);

    // 'delete' the shaders so they are destroyed when the program is destroyed
    glDeleteShader(vs);
    glDeleteShader(fs);

    glLinkProgram(id);

    GLint status = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == 0) {
        int logLength = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::string log;
            log.resize((size_t) logLength);
            glGetShaderInfoLog(id, logLength, NULL, &log[0]);
            throw std::runtime_error("Error linking program: " + log);
        }
    }

    glUseProgram(id);
    posAttr = glGetAttribLocation(id, "position");
    colAttr = glGetAttribLocation(id, "color");
    uvAttr = glGetAttribLocation(id, "uv");
    projUni = glGetUniformLocation(id, "projection");
    samplerUni = glGetUniformLocation(id, "textureSampler");
}

ShaderProgram::ShaderProgram(ShaderProgram&& prog) : id(prog.id), posAttr(prog.posAttr), colAttr(prog.colAttr),
                                                     uvAttr(prog.uvAttr), projUni(prog.projUni) {
    prog.hasId = false;
    prog.id = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& prog) {
    if (hasId)
        glDeleteProgram(id);
    id = prog.id;
    hasId = prog.hasId;
    posAttr = prog.posAttr;
    colAttr = prog.colAttr;
    uvAttr = prog.uvAttr;
    projUni = prog.projUni;
    samplerUni = prog.samplerUni;
    prog.hasId = false;
    prog.id = 0;
    return *this;
}

ShaderProgram::~ShaderProgram() {
    if (hasId)
        glDeleteProgram(id);
}

ShaderProgram ShaderProgram::fromFiles(std::string vertexPath, std::string fragmentPath) {
    std::ifstream vs (vertexPath);
    std::stringstream vss;
    vss << vs.rdbuf();
    std::ifstream fs (fragmentPath);
    std::stringstream fss;
    fss << fs.rdbuf();
    return ShaderProgram(vss.str(), fss.str());
}