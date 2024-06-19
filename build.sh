#!/bin/bash

# Set the build directory
BUILD_DIR="build/Release"

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Generate the build files using CMake, specifying the build type as Release
cmake ../.. -DCMAKE_BUILD_TYPE=Release

# Build the project
make

# Return to the original directory
cd ../..