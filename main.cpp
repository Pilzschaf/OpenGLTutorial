#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
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

#include "defines.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "shader.h"
#include "floating_camera.h"
#include "mesh.h"

void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << "[OpenGL Error] " << message << std::endl;
}

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

	Shader shader("shaders/basic.vs", "shaders/basic.fs");
	shader.bind();
	int directionLocation = GLCALL(glGetUniformLocation(shader.getShaderId(), "u_directional_light.direction"));
	glm::vec3 sunColor = glm::vec3(0.8f);
	glm::vec3 sunDirection = glm::vec3(-1.0f);
	GLCALL(glUniform3fv(glGetUniformLocation(shader.getShaderId(), "u_directional_light.diffuse"), 1, (float*)&sunColor.data));
	GLCALL(glUniform3fv(glGetUniformLocation(shader.getShaderId(), "u_directional_light.specular"), 1, (float*)&sunColor.data));
	sunColor *= 0.4f;
	GLCALL(glUniform3fv(glGetUniformLocation(shader.getShaderId(), "u_directional_light.ambient"), 1, (float*)&sunColor.data));

	glm::vec3 pointLightColor = glm::vec3(0.0f, 0.0f, 1.0f);
	GLCALL(glUniform3fv(glGetUniformLocation(shader.getShaderId(), "u_point_light.diffuse"), 1, (float*)&pointLightColor.data));
	GLCALL(glUniform3fv(glGetUniformLocation(shader.getShaderId(), "u_point_light.specular"), 1, (float*)&pointLightColor.data));
	pointLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(shader.getShaderId(), "u_point_light.ambient"), 1, (float*)&pointLightColor.data));
	GLCALL(glUniform1f(glGetUniformLocation(shader.getShaderId(), "u_point_light.linear"), 0.027f));
	GLCALL(glUniform1f(glGetUniformLocation(shader.getShaderId(), "u_point_light.quadratic"), 0.0028f));
	glm::vec4 pointLightPosition = glm::vec4(0.0f, 0.0f, 10.0f, 1.0f);
	int positionLocation = GLCALL(glGetUniformLocation(shader.getShaderId(), "u_point_light.position"));
	
	Model monkey;
	monkey.init("models/tree.bmf", &shader);

	uint64 perfCounterFrequency = SDL_GetPerformanceFrequency();
	uint64 lastCounter = SDL_GetPerformanceCounter();
	float32 delta = 0.0f;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.1f));

	FloatingCamera camera(90.0f, 800.0f, 600.0f);
	camera.translate(glm::vec3(0.0f, 0.0f, 5.0f));
	camera.update();

	glm::mat4 modelViewProj = camera.getViewProj() * model;

	int modelViewProjMatrixLocation = GLCALL(glGetUniformLocation(shader.getShaderId(), "u_modelViewProj"));
	int modelViewLocation = GLCALL(glGetUniformLocation(shader.getShaderId(), "u_modelView"));
	int invModelViewLocation = GLCALL(glGetUniformLocation(shader.getShaderId(), "u_invModelView"));

	// Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	bool buttonW = false;
	bool buttonS = false;
	bool buttonA = false;
	bool buttonD = false;
	bool buttonSpace = false;
	bool buttonShift = false;

	float cameraSpeed = 6.0f;
	float time = 0.0f;
	bool close = false;
	SDL_SetRelativeMouseMode(SDL_TRUE);
	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
	while(!close) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				close = true;
			} else if(event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
					case SDLK_w:
					buttonW = true;
					break;
					case SDLK_s:
					buttonS = true;
					break;
					case SDLK_a:
					buttonA = true;
					break;
					case SDLK_d:
					buttonD = true;
					break;
					case SDLK_SPACE:
					buttonSpace = true;
					break;
					case SDLK_LSHIFT:
					buttonShift = true;
					break;
					case SDLK_ESCAPE:
					SDL_SetRelativeMouseMode(SDL_FALSE);
					break;
				}
			} else if(event.type == SDL_KEYUP) {
				switch(event.key.keysym.sym)  {
					case SDLK_w:
					buttonW = false;
					break;
					case SDLK_s:
					buttonS = false;
					break;
					case SDLK_a:
					buttonA = false;
					break;
					case SDLK_d:
					buttonD = false;
					break;
					case SDLK_SPACE:
					buttonSpace = false;
					break;
					case SDLK_LSHIFT:
					buttonShift = false;
					break;
				}
			} else if(event.type == SDL_MOUSEMOTION) {
				if(SDL_GetRelativeMouseMode()) {
					camera.onMouseMoved(event.motion.xrel, event.motion.yrel);
				}
			} else if(event.type == SDL_MOUSEBUTTONDOWN) {
				if(event.button.button == SDL_BUTTON_LEFT) {
					SDL_SetRelativeMouseMode(SDL_TRUE);
				}
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		time += delta;

		if(buttonW) {
			camera.moveFront(delta * cameraSpeed);
		}
		if(buttonS) {
			camera.moveFront(-delta * cameraSpeed);
		}if(buttonA) {
			camera.moveSideways(-delta * cameraSpeed);
		}if(buttonD) {
			camera.moveSideways(delta * cameraSpeed);
		}if(buttonSpace) {
			camera.moveUp(delta * cameraSpeed);
		}if(buttonShift) {
			camera.moveUp(-delta * cameraSpeed);
		}

		camera.update();
		model = glm::rotate(model, 1.0f*delta, glm::vec3(0, 1, 0));
		modelViewProj = camera.getViewProj() * model;
		glm::mat4 modelView = camera.getView() * model;
		glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));
		
		glm::vec4 transformedSunDirection = glm::transpose(glm::inverse(camera.getView())) * glm::vec4(sunDirection, 1.0f);
		glUniform3fv(directionLocation, 1, (float*)&transformedSunDirection.data);

		glm::mat4 pointLightMatrix = glm::rotate(glm::mat4(1.0f), -delta, {0.0f, 1.0f, 0.0f});
		pointLightPosition = pointLightMatrix * pointLightPosition;
		glm::vec3 transformedPointLightPosition = (glm::vec3) (camera.getView() * pointLightPosition);
		glUniform3fv(positionLocation, 1, (float*)&transformedPointLightPosition.data);

		GLCALL(glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE, &modelViewProj[0][0]));
		GLCALL(glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, &modelView[0][0]));
		GLCALL(glUniformMatrix4fv(invModelViewLocation, 1, GL_FALSE, &invModelView[0][0]));
		monkey.render();

		SDL_GL_SwapWindow(window);

		uint64 endCounter = SDL_GetPerformanceCounter();
		uint64 counterElapsed = endCounter - lastCounter;
		delta = ((float32)counterElapsed) / (float32)perfCounterFrequency;
		uint32 FPS = (uint32)((float32)perfCounterFrequency / (float32)counterElapsed);
		lastCounter = endCounter;
	}

	return 0;
}