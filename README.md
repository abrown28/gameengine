# Game Engine with Raylib, ENet, EnTT, and Boost

A C++ game engine that combines the power of [Raylib](https://www.raylib.com/) for graphics, [ENet](http://enet.bespin.org/) for networking, [EnTT](https://github.com/skypjack/entt) for entity component system (ECS), and [Boost](https://www.boost.org/) for additional C++ libraries. This project demonstrates how to create a networked game with modern C++ and CMake.

## Features

- **Graphics**: Raylib for 2D graphics, input handling, and window management
- **Networking**: ENet for reliable UDP networking with client-server architecture
- **ECS**: EnTT for fast and flexible entity component system
- **Boost Libraries**: Filesystem, Thread, Chrono, Regex, Date/Time, and more
- **Modern C++**: Uses C++17 features and smart pointers
- **Cross-platform**: Works on Windows, Linux, and macOS
- **CMake Build**: Easy to build and configure

## Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Git (for downloading dependencies)

## Building the Project

### Windows (Visual Studio)

```bash
# Option 1: Use the provided batch script
build_with_boost.bat

# Option 2: Manual build
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 16 2019" -A x64

# Build the project
cmake --build . --config Release
```

### Windows (MinGW)

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "MinGW Makefiles"

# Build the project
cmake --build .
```

### Linux/macOS

```bash
# Option 1: Use the provided shell script
chmod +x build_with_boost.sh
./build_with_boost.sh

# Option 2: Manual build
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make
```

## Running the Application

After building, you can run the application:

```bash
# On Windows
./build/Release/GameEngine.exe

# On Linux/macOS
./build/GameEngine
```

## Controls

- **WASD** or **Arrow Keys**: Move the player
- **ESC**: Exit the application

## Project Structure

```
GameEngine/
├── CMakeLists.txt          # Main CMake configuration
├── README.md              # This file
├── include/               # Header files
│   ├── Game.h            # Game class header
│   ├── NetworkManager.h  # Network manager header
│   └── ECS.h             # Entity Component System header
├── src/                  # Source files
│   ├── main.cpp          # Application entry point
│   ├── Game.cpp          # Game class implementation
│   ├── NetworkManager.cpp # Network manager implementation
│   └── ECS.cpp           # Entity Component System implementation
└── assets/               # Game assets (if any)
```

## How It Works

### Graphics (Raylib)
- The `Game` class uses Raylib for rendering a simple 2D scene
- Displays a player character (white circle) that can be moved with keyboard input
- Shows real-time information about player position and network status

### Networking (ENet)
- The `NetworkManager` class handles all networking functionality
- Supports both client and server modes
- Uses reliable UDP packets for position synchronization
- Automatically starts as a server on port 12345

### Entity Component System (EnTT)
- The `ECSSystem` class provides a fast and flexible ECS implementation
- Components include: `ECSTransform`, `Velocity`, `Renderable`, `Player`, and `Networked`
- Systems handle movement, rendering, and network synchronization
- Demonstrates entity creation, component management, and system updates

### Game Loop
1. **Initialize**: Set up Raylib window and ENet networking
2. **Update**: Process input, update game state, and handle network events
3. **Render**: Draw the game scene using Raylib
4. **Shutdown**: Clean up resources

## Dependencies

The project automatically downloads and builds these dependencies:

- **Raylib 5.5**: Modern and easy-to-use game programming library
- **ENet 1.3.18**: Reliable UDP networking library
- **EnTT 3.12.2**: Fast and flexible entity component system library
- **Boost 1.84.0**: Comprehensive C++ libraries including:
  - **Filesystem**: Cross-platform file system operations
  - **Thread**: Multi-threading support
  - **Chrono**: Time utilities and clocks
  - **Regex**: Regular expressions
  - **Date/Time**: Date and time manipulation
  - **Serialization**: Object serialization
  - **Program Options**: Command line argument parsing

## Customization

### Adding New Features

1. **Graphics**: Add new drawing functions in `Game::Render()`
2. **Input**: Extend `Game::HandleInput()` for new controls
3. **Networking**: Modify `NetworkManager` to handle new message types
4. **ECS**: Add new components and systems in `ECS.h` and `ECS.cpp`
5. **Game Logic**: Add new game mechanics in `Game::Update()`
6. **Boost Libraries**: Utilize Boost libraries for advanced functionality:
   - **Filesystem**: File and directory operations
   - **Thread**: Multi-threading and synchronization
   - **Chrono**: High-precision timing
   - **Regex**: Pattern matching and text processing
   - **Date/Time**: Calendar and time calculations

### Network Protocol

The current implementation uses a simple text-based protocol:
- `POS:x,y`: Player position updates
- Extend this by adding new message types in `NetworkManager`

## Troubleshooting

### Common Issues

1. **CMake not found**: Install CMake from https://cmake.org/
2. **Compiler not found**: Install a C++17 compatible compiler
3. **Build errors**: Ensure all prerequisites are installed
4. **Network issues**: Check firewall settings for port 12345

### Platform-Specific Notes

- **Windows**: May need to run as administrator for server mode
- **Linux**: May need to install additional development packages
- **macOS**: Works out of the box with Xcode Command Line Tools

## License

This project is open source. Feel free to modify and distribute according to your needs.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Acknowledgments

- [Raylib](https://www.raylib.com/) - The game programming library
- [ENet](http://enet.bespin.org/) - The networking library
- [EnTT](https://github.com/skypjack/entt) - The entity component system library
- [Boost](https://www.boost.org/) - The C++ libraries collection
- CMake community for the build system
