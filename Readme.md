EdgePredict - Core Simulation Engine
This project is built using CMake, the cross-platform standard for C++ projects.

Prerequisites
CMake: Installed and available in your terminal's PATH.

C++ Compiler: A modern g++ compiler from MSYS2.

Required Libraries: json.hpp and the Eigen folder are placed inside the include/ directory.

Compilation & Building (The CMake Way)
Run all the following commands from the root edgepredict-engine directory.

1. Clean the Environment (Important)
Before starting, always ensure you have a clean state by removing any old build directories.

rm -rf build

2. Configure the Build (The New, Explicit Method)
Create a new build directory and navigate into it.

mkdir build
cd build

Now, run CMake, but this time, we will explicitly tell it to generate "MinGW Makefiles" using the -G flag.

cmake -G "MinGW Makefiles" ..

You should see output as CMake configures the project, ending with "Configuring done" and "Generating done." If you run ls now, you will see a Makefile has been created inside the build directory.

3. Build the Executable
Now that the Makefile has been generated, you can compile the project by running the make command from inside the build directory.

make

This will compile your code and place the final edgepredict_engine.exe file inside the build/ directory.

Running a Simulation
From inside the build/ directory, run the engine:

./edgepredict_engine.exe ../input.json
