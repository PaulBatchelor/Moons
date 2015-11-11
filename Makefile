#ifndef system
system ?= $(shell uname -s)
#endif

ifeq ($(system), Darwin)
CXX=clang++
CXXFLAGS=-D__MACOSX_CORE__ -c
CFLAGS=-D__MACOSX_CORE__ -c
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit -lstdc++ -lm -L/usr/local/lib
CXXFLAGS += -I/usr/local/include
CFLAGS += -I/usr/local/include
else
CXX=g++
CXXFLAGS=-D__UNIX_JACK__ -c
LIBS= -ljack -lGL -lGLU -lglut -lstdc++ -lm -lpthread
endif

SR=96000

ifdef SR
CFLAGS += -DMY_SRATE=$(SR)
CXXFLAGS += -DMY_SRATE=$(SR)
endif

LIBS+=-Llibs -lsporth -lsoundpipe -lsndfile
CFLAGS += -Iinclude 
CXXFLAGS += -Iinclude 

OBJ=main.o draw.o RtAudio.o audio.o 

default: moons

moons: $(OBJ) 
	$(CXX) $^ $(LIBS) -o $@ 

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $^

clean:
	rm -rf $(OBJ) moons
