#include <iostream>
#include <cmath>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#ifdef _WIN32
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#else
#include <SDL2/SDL.h>
#endif

#include "defines.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "shader.h"

void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << "[OpenGL Error] " << message << std::endl;
}

#ifdef _DEBUG

void _GLGetError(const char* file, int line, const char* call) {
	while(GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] " << glewGetErrorString(error) << " in " << file << ":" << line << " Call: " << call << std::endl;
	}
}

#define GLCALL(call) call; _GLGetError(__FILE__, __LINE__, #call)

#else

#define GLCALL(call) call

#endif

int main(int argc, char** argv) {
	SDL_Window* window;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif

	uint32 flags = SDL_WINDOW_OPENGL;

	window = SDL_CreateWindow("C++ OpenGL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if(err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		std::cin.get();
		return -1;
	}
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, 0);
	#endif

	Vertex vertices[] = {
		Vertex{-0.5f, -0.5f, -0.0f,
		1.0f, 0.0f, 0.0f, 1.0f},
		Vertex{0.5f, -0.5f, -0.0f,
		0.0, 1.0f, 0.0f, 1.0f},
		Vertex{0.0f, 0.5f, -0.0f,
		0.0f, 0.0f, 1.0f, 1.0f}
	};
	uint32 numVertices = 3;

	uint32 indices[] = {
		0, 1, 2
	};
	uint32 numIndices = 3;

	IndexBuffer indexBuffer(indices, numIndices, sizeof(indices[0]));

	VertexBuffer vertexBuffer(vertices, numVertices);
	vertexBuffer.unbind();

	Shader shader("shaders/basic.vs", "shaders/basic.fs");
	shader.bind();

	uint64 perfCounterFrequency = SDL_GetPerformanceFrequency();
	uint64 lastCounter = SDL_GetPerformanceCounter();
	float32 delta = 0.0f;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.2f));

	glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -10.0f, 100.0f);
	projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);

	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

	glm::mat4 modelViewProj = projection * view * model;

	int modelViewProjMatrixLocation = GLCALL(glGetUniformLocation(shader.getShaderId(), "u_modelViewProj"));

	// Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float time = 0.0f;
	bool close = false;
	while(!close) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		time += delta;

		model = glm::rotate(model, 1.0f*delta, glm::vec3(0, 1, 0));
		modelViewProj = projection * view * model;

		vertexBuffer.bind();
		indexBuffer.bind();
		GLCALL(glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE, &modelViewProj[0][0]));
		GLCALL(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));
		indexBuffer.unbind();
		vertexBuffer.unbind();

		SDL_GL_SwapWindow(window);
		
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				close = true;
			}
		}

		uint64 endCounter = SDL_GetPerformanceCounter();
		uint64 counterElapsed = endCounter - lastCounter;
		delta = ((float32)counterElapsed) / (float32)perfCounterFrequency;
		uint32 FPS = (uint32)((float32)perfCounterFrequency / (float32)counterElapsed);
		lastCounter = endCounter;
	}

	return 0;
}