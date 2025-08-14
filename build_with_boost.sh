#!/bin/bash

echo "Building Game Engine with Boost libraries..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build completed successfully!"
echo "Executable location: build/GameEngine"
