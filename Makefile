COMPILER = g++

MORPHOLOGICA_PATH = /home/jakub/Documents/morphologica-main

SHARED_LIBS = /usr/lib/x86_64-linux-gnu/libglfw.so.3.3 /usr/lib/x86_64-linux-gnu/libOpenGL.so /usr/lib/x86_64-linux-gnu/libfreetype.so /usr/lib/x86_64-linux-gnu/libGLX.so /usr/lib/x86_64-linux-gnu/libhdf5_cpp.so.103 /usr/lib/x86_64-linux-gnu/libhdf5_serial.so

COMPILER_SETTINGS = -Wall -pedantic -g -Wfatal-errors -Wno-unused-result -Wno-unknown-pragmas -march=native -O3 -fopenmp -std=gnu++17

COMPILER_REQ_FLAGS = -D__GLN__ -DGL3_PROTOTYPES -DGL_GLEXT_PROTOTYPES -DMORPH_FONTS_DIR="\"$(MORPHOLOGICA_PATH)/fonts\""

REQUIRED_INCLUDES = -I$(MORPHOLOGICA_PATH) -I$(MORPHOLOGICA_PATH)/include -isystem /usr/include/jsoncpp -isystem /usr/include/freetype2 -isystem /usr/include/hdf5/serial 

.PHONY = all debug visual experiments clean test

all: 
	$(COMPILER) $(REQUIRED_INCLUDES) $(COMPILER_REQ_FLAGS) $(COMPILER_SETTINGS) -o testVisual testVisual.cpp $(SHARED_LIBS)
	$(COMPILER) $(REQUIRED_INCLUDES) $(COMPILER_REQ_FLAGS) $(COMPILER_SETTINGS) -o testLoad testLoad.cpp $(SHARED_LIBS)
	$(COMPILER) $(REQUIRED_INCLUDES) $(COMPILER_REQ_FLAGS) $(COMPILER_SETTINGS) -o testSave testSave.cpp $(SHARED_LIBS)

visual:
	$(COMPILER) $(REQUIRED_INCLUDES) $(COMPILER_REQ_FLAGS) $(COMPILER_SETTINGS) -o testVisual testVisual.cpp $(SHARED_LIBS)

experiments:
	$(COMPILER) $(REQUIRED_INCLUDES) $(COMPILER_REQ_FLAGS) $(COMPILER_SETTINGS) -o testExperiments testExperiments.cpp $(SHARED_LIBS)

clean:
	rm -f testVisual testLoad testSave testExperiments