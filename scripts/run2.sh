#!/bin/bash
# This script runs the DECODER functionality of the metroPlanner executable.
# It takes a basename as an argument (e.g., "test"), reads "test.city"
# and "test.satoutput", and produces "test.metromap".

set -e

# Check for the correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: ./run2.sh <basename>"
    echo "Example: ./run2.sh test"
    exit 1
fi

BASENAME=$1
INPUT_CITY="${BASENAME}.city"
INPUT_SATOUTPUT="${BASENAME}.satoutput"
OUTPUT_METROMAP="${BASENAME}.metromap"

# Check if the necessary files exist
if [ ! -f metroPlanner ]; then
    echo "Error: Executable 'metroPlanner' not found. Please run compile.sh first."
    exit 1
fi
if [ ! -f "$INPUT_CITY" ]; then
    echo "Error: Input file '${INPUT_CITY}' not found."
    exit 1
fi
if [ ! -f "$INPUT_SATOUTPUT" ]; then
    echo "Error: MiniSAT output file '${INPUT_SATOUTPUT}' not found."
    exit 1
fi

echo "Running decoder..."

# We assume your C++ main.cpp is structured to handle these arguments:
# ./metroPlanner decode <input.city> <input.satoutput> <output.metromap>
./metroPlanner decode "$INPUT_CITY" "$INPUT_SATOUTPUT" "$OUTPUT_METROMAP"

echo "Decoding complete. Output written to '${OUTPUT_METROMAP}'."