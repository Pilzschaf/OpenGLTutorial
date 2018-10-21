#pragma once
#include <windows.h>
#include <GL/GL.h>

// Context must be loaded
void loadOpenGLFunctions();

#define GL_STATIC_DRAW 0x88E4
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_SAMPLES_PASSED 0x8914
#define GL_COMPILE_STATUS 0x8B81
#define GL_INFO_LOG_LENGTH 0x8b84
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;

typedef void WINAPI wglGenVertexArrays(GLsizei n, GLuint *arrays);
typedef void WINAPI wglBindVertexArray(GLuint array);
typedef void WINAPI wglGenBuffers(GLsizei n, GLuint * buffers);
typedef void WINAPI wglBindBuffer(GLenum target, GLuint buffer);
typedef void WINAPI wglBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
typedef void WINAPI wglEnableVertexAttribArray(GLuint index);
typedef void WINAPI wglVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
typedef void WINAPI wglDeleteBuffers(GLsizei n, const GLuint * buffers);
typedef void WINAPI wglDeleteProgram(GLuint program);
typedef void WINAPI wglUseProgram(GLuint program);
typedef GLuint WINAPI wglCreateShader(GLenum shaderType);
typedef void WINAPI wglShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
typedef void WINAPI wglCompileShader(GLuint shader);
typedef void WINAPI wglGetShaderiv(GLuint shader, GLenum pname, GLint *params);
typedef void WINAPI wglGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void WINAPI wglAttachShader(GLuint program, GLuint shader);
typedef void WINAPI wglLinkProgram(GLuint program);
typedef void WINAPI wglDetachShader(GLuint program, GLuint shader);
typedef void WINAPI wglDeleteShader(GLuint shader);
typedef GLuint WINAPI wglCreateProgram(void);

extern wglGenVertexArrays* glGenVertexArrays;
extern wglBindVertexArray* glBindVertexArray;
extern wglGenBuffers* glGenBuffers;
extern wglBindBuffer* glBindBuffer;
extern wglBufferData* glBufferData;
extern wglEnableVertexAttribArray* glEnableVertexAttribArray;
extern wglVertexAttribPointer* glVertexAttribPointer;
extern wglDeleteBuffers* glDeleteBuffers;
extern wglDeleteProgram* glDeleteProgram;
extern wglUseProgram* glUseProgram;
extern wglCreateShader* glCreateShader;
extern wglShaderSource* glShaderSource;
extern wglCompileShader* glCompileShader;
extern wglGetShaderiv* glGetShaderiv;
extern wglGetShaderInfoLog* glGetShaderInfoLog;
extern wglAttachShader* glAttachShader;
extern wglLinkProgram* glLinkProgram;
extern wglDetachShader* glDetachShader;
extern wglDeleteShader* glDeleteShader;
extern wglCreateProgram* glCreateProgram;