#!/bin/bash

# Get the parent directory of this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check for the correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: ./run.sh <basename>"
    echo "Example: ./run.sh test"
    exit 1
fi

# Run the scripts using absolute paths
"$SCRIPT_DIR/compile.sh"
"$SCRIPT_DIR/run1.sh" "$1"
minisat "$1.satinput" "$1.satoutput"
"$SCRIPT_DIR/run2.sh" "$1"
