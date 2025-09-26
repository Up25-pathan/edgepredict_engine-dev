# Makefile for the EdgePredict Core Engine with CUDA support

# Compilers
CXX = g++
NVCC = nvcc

# Compiler flags for standard C++
CXXFLAGS = -std=c++17 -Wall -I./include

# Compiler flags for CUDA C++. 
# -arch=sm_75 is a common setting for modern GPUs, you might adjust this for older cards.
NVCCFLAGS = -std=c++17 -arch=sm_75 -I./include

# Source files
CXX_SRCS = src/main.cpp src/simulation.cpp
CU_SRCS = src/solver.cu

# Automatically generate object file names from source file names
OBJS = $(CXX_SRCS:.cpp=.o) $(CU_SRCS:.cu=.o)

# The final executable name
TARGET = edgepredict_engine

# The default command to run when you type 'make'
all: $(TARGET)

# Rule to link the final executable from all the object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -L/usr/local/cuda/lib64 -lcudart

# Rule to compile standard C++ source files (.cpp) into object files (.o)
%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule to compile CUDA source files (.cu) into object files (.o)
%.o: src/%.cu
	$(NVCC) $(NVCCFLAGS) -c -o $@ $<

# A command to clean up all compiled files
clean:
	rm -f $(TARGET) $(OBJS)


