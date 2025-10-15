#!/bin/bash
# This script compiles the C++ source files from the src/ directory
# into a single executable in the root directory.

# Exit immediately if a command exits with a non-zero status.
set -e

echo "Compiling C++ source files..."

# Compile all .cpp files in src/ and link them into one executable
g++ -std=c++11 -O2 -Wall src/main.cpp src/satEncoder.cpp -o metroPlanner

echo "Compilation successful. Executable 'metroPlanner' is ready."