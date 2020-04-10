#CC=g++

OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/usr/local/lib 
CPPFLAGS=-I/usr/local/include 

BIN=camera
SRC1=skybox.cpp GL_framework/gl_framework.cpp shaders/shader_util.cpp ../stb_image.cpp 
INCLUDES=GL_framework/gl_framework.hpp shaders/shader_util.hpp skybox.hpp ../stb_image.h

all: $(BIN)

$(BIN): $(SRC1) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRC1) -o $(BIN) $(LDFLAGS) $(LIBS)

clean:
	rm -f *~ *.o $(BIN)
