cmake_minimum_required(VERSION 4.1.0)
project(GameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Use FetchContent to download and build dependencies
include(FetchContent)

# Download and build raylib (using a more recent version)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
# Disable building examples, demos, games, and tools to avoid CMake version issues
set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# Download and build enet
FetchContent_Declare(
    enet
    GIT_REPOSITORY https://github.com/lsalzman/enet.git
    GIT_TAG v1.3.18
)
FetchContent_MakeAvailable(enet)

# Download and build EnTT
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.12.2
)
FetchContent_MakeAvailable(entt)

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/Game.cpp
    src/NetworkManager.cpp
    src/ECS.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${raylib_SOURCE_DIR}/src
    ${enet_SOURCE_DIR}/include
    ${entt_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    raylib
    enet
    EnTT::EnTT
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(${PROJECT_NAME} winmm ws2_32)
    # Define preprocessor macros to avoid Windows API conflicts
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        PLATFORM_DESKTOP
        GRAPHICS_API_OPENGL_33
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WINSOCK_DEPRECATED_NO_WARNINGS
        NOGDI
        NOUSER
    )
endif()

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
