#!/bin/bash

source ~/emsdk/emsdk_env.sh

rm -rf build_web
mkdir build_web

echo "Building Game Engine for Web with Emscripten..."

# Check if Emscripten is available
if ! command -v emcc &> /dev/null; then
    echo "ERROR: Emscripten not found in PATH!"
    echo "Please install Emscripten and add it to your PATH, or run:"
    echo "  source /path/to/emsdk/emsdk_env.sh"
    echo "from your Emscripten installation directory."
    exit 1
fi

# Check if emcmake is available
if ! command -v emcmake &> /dev/null; then
    echo "ERROR: emcmake not found in PATH!"
    echo "Please ensure Emscripten is properly installed and activated."
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "build_web" ]; then
    mkdir build_web
fi
cd build_web

# Configure with Emscripten CMake
echo "Configuring with Emscripten CMake..."
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the web target
echo "Building web target..."
emmake make GameEngine_web
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Web build completed successfully!"
echo ""
echo "Generated files:"
echo "  GameEngine.html - Main HTML file"
echo "  GameEngine.js   - JavaScript glue code"
echo "  GameEngine.wasm - WebAssembly binary"
echo "  web/assets/     - Game assets"
echo ""
echo "To test the web build:"
echo "  1. Start a local web server in the build_web directory"
echo "  2. Open GameEngine.html in a web browser"
echo ""
echo "Example using Python:"
echo "  python3 -m http.server 8000"
echo "  Then open: http://localhost:8000/GameEngine.html"
echo ""
echo "Example using Node.js:"
echo "  npx http-server -p 8000"
echo "  Then open: http://localhost:8000/GameEngine.html"
echo ""
