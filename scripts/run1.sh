#!/bin/bash
# This script runs the ENCODER functionality of the metroPlanner executable.
# It takes a basename as an argument (e.g., "test"), reads "test.city",
# and produces "test.satinput".

set -e

# Check for the correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: ./run1.sh <basename>"
    echo "Example: ./run1.sh test"
    exit 1
fi

BASENAME=$1
INPUT_CITY="${BASENAME}.city"
OUTPUT_SATINPUT="${BASENAME}.satinput"

# Check if the executable and input file exist
if [ ! -f metroPlanner ]; then
    echo "Error: Executable 'metroPlanner' not found. Please run compile.sh first."
    exit 1
fi
if [ ! -f "$INPUT_CITY" ]; then
    echo "Error: Input file '${INPUT_CITY}' not found."
    exit 1
fi

echo "Running encoder..."

# We assume your C++ main.cpp is structured to handle these arguments:
# ./metroPlanner encode <input.city> <output.satinput>
./metroPlanner encode "$INPUT_CITY" "$OUTPUT_SATINPUT"

echo "Encoding complete. Output written to '${OUTPUT_SATINPUT}'."