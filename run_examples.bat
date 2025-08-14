@echo off
echo Game Engine Command Line Examples
echo =================================

echo.
echo 1. Start server mode:
echo build\Release\GameEngine.exe --server --port 12345 --verbose

echo.
echo 2. Start client mode (default):
echo build\Release\GameEngine.exe --client --host 127.0.0.1 --port 12345

echo.
echo 3. Start in fullscreen mode:
echo build\Release\GameEngine.exe --fullscreen --width 1920 --height 1080

echo.
echo 4. Show help:
echo build\Release\GameEngine.exe --help

echo.
echo 5. Short options:
echo build\Release\GameEngine.exe -s -p 12345 -v

echo.
echo 6. Connect to remote server:
echo build\Release\GameEngine.exe --host 192.168.1.100 --port 12345

echo.
pause
