#include <Windows.h>
#include <iostream>
#include <cassert>
#include <cstdint>

#include "vertex_buffer.h"
#include "shader.h"

#pragma comment(lib, "opengl32.lib")

LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int main() {
	HINSTANCE hInstance = GetModuleHandle(0);
	HWND hWnd;
	WNDCLASS wc;
	MSG msg;

	wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MessageHandler;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "WINAPITest";

	assert(RegisterClass(&wc));

	hWnd = CreateWindow("WINAPITest", "WinAPI Tutorial", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, hInstance, 0);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	std::cout << "Created window" << std::endl;

	HDC hdc = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pixelFormat = {};
	pixelFormat.nSize = sizeof(pixelFormat);
	pixelFormat.nVersion = 1;
	pixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pixelFormat.cColorBits = 24;
	pixelFormat.cRedBits = 8;
	pixelFormat.cGreenBits = 8;
	pixelFormat.cBlueBits = 8;
	pixelFormat.cAlphaBits = 8;

	int index = ChoosePixelFormat(hdc, &pixelFormat);
	PIXELFORMATDESCRIPTOR supportedPixelFormat;
	DescribePixelFormat(hdc, index, sizeof(PIXELFORMATDESCRIPTOR), &supportedPixelFormat);
	SetPixelFormat(hdc, index, &supportedPixelFormat);

	HGLRC context = wglCreateContext(hdc);
	if (!wglMakeCurrent(hdc, context)) {
		std::cout << "OpenGL context creation failed" << std::endl;
		assert(false);
		return 0;
	}
	
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	loadOpenGLFunctions();

	Vertex vertices[] = {
		Vertex{-0.5f, -0.5f, 0.0f},
		Vertex{0.0f, 0.5f, 0.0f},
		Vertex{0.5f, -0.5f, 0.0f}
	};
	uint32 numVertices = 3;

	VertexBuffer vertexBuffer(vertices, numVertices);
	vertexBuffer.unbind();

	Shader shader("shaders/basic.vs", "shaders/basic.fs");
	shader.bind();

	bool running = true;
	while (running) {
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		vertexBuffer.bind();
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		vertexBuffer.unbind();

		SwapBuffers(hdc);
	}
}