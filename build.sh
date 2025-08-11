#!/bin/bash

echo "Building Game Engine with Raylib and ENet..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "Building project..."
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build completed successfully!"
echo ""
echo "To run the application:"
echo "  ./GameEngine"
echo ""
