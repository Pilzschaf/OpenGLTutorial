#pragma once
#include <GL/glew.h>
#include <string>
#include "defines.h"

struct Shader {
    Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename);
    virtual ~Shader();

    void bind();
    void unbind();

    GLuint getShaderId() {
        return shaderId;
    }

private:

    GLuint compile(std::string shaderSource, GLenum type);
    std::string parse(const char* filename);
    GLuint createShader(const char* vertexShaderFilename, const char* fragmentShaderFilename);

    GLuint shaderId;
};