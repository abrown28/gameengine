@echo off
echo Building Game Engine for Web with Emscripten...

REM Check if Emscripten is available
where emcc >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Emscripten not found in PATH!
    echo Please install Emscripten and add it to your PATH, or run:
    echo   emsdk_env.bat
    echo from your Emscripten installation directory.
    pause
    exit /b 1
)

REM Check if emcmake is available
where emcmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: emcmake not found in PATH!
    echo Please ensure Emscripten is properly installed and activated.
    pause
    exit /b 1
)

REM Create build directory if it doesn't exist
if not exist "build_web" mkdir build_web
cd build_web

REM Configure with Emscripten CMake
echo Configuring with Emscripten CMake...
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the web target
echo Building web target...
emmake make GameEngine_web
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Web build completed successfully!
echo.
echo Generated files:
echo   GameEngine.html - Main HTML file
echo   GameEngine.js   - JavaScript glue code
echo   GameEngine.wasm - WebAssembly binary
echo   web/assets/     - Game assets
echo.
echo To test the web build:
echo   1. Start a local web server in the build_web directory
echo   2. Open GameEngine.html in a web browser
echo.
echo Example using Python:
echo   python -m http.server 8000
echo   Then open: http://localhost:8000/GameEngine.html
echo.
pause
