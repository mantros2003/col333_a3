#!/bin/bash
# This script compiles the C++ source files from the src/ directory
# into a single executable in the root directory.

# Exit immediately if a command exits with a non-zero status.
set -e

echo "Compiling SAT encoder..."

# Compile all .cpp files in src/ and link them into one executable
g++ -std=c++11 -O2 -Wall src/main.cpp src/satEncoder.cpp -o encoder

echo "Compiling SAT decoder..."

g++ -std=c++11 -O2 -Wall src/satDecoder.cpp -o decoder

echo "Compilation successful. Executable 'metroPlanner' is ready."