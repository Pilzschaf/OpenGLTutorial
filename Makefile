CXXARGS = -g -std=c++11 -D _DEBUG

all : opengl_tutorial tools/modelexporter

opengl_tutorial : 
	g++ $(CXXARGS) main.cpp shader.cpp -o opengl_tutorial -lGL -lSDL2 -lGLEW

tools/modelexporter :
	g++ $(CXXARGS) tools/modelexporter.cpp -o tools/modelexporter -lassimp

clean : 
	rm opengl_tutorial tools/modelexporter