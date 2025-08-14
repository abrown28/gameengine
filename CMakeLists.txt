cmake_minimum_required(VERSION 3.10)
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

# Configure raylib based on platform
if(EMSCRIPTEN)
    # Emscripten-specific raylib settings
    set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(RAYLIB_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(RAYLIB_BUILD_GAMES OFF CACHE BOOL "" FORCE)
    set(RAYLIB_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(PLATFORM Web CACHE STRING "" FORCE)
    set(GRAPHICS GRAPHICS_API_OPENGL_ES2 CACHE STRING "" FORCE)
else()
    # Desktop-specific raylib settings
    set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(RAYLIB_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(RAYLIB_BUILD_GAMES OFF CACHE BOOL "" FORCE)
    set(RAYLIB_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(PLATFORM Desktop CACHE STRING "" FORCE)
    set(GRAPHICS GRAPHICS_API_OPENGL_33 CACHE STRING "" FORCE)
endif()

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

# Download and build Boost
FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.84.0
)

# Configure Boost
set(BOOST_ENABLE_CMAKE ON CACHE BOOL "" FORCE)
set(BOOST_INSTALL_LIBRARIES ON CACHE BOOL "" FORCE)

# Specify which Boost libraries to build (common ones)
set(BOOST_LIBRARIES 
    system
    filesystem
    thread
    chrono
    date_time
    regex
    serialization
    program_options
    CACHE STRING "" FORCE
)

FetchContent_MakeAvailable(boost)

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/Game.cpp
    src/NetworkManager.cpp
    src/ECS.cpp
)

# Emscripten target for web deployment
if(EMSCRIPTEN)
    add_executable(${PROJECT_NAME}_web
        src/main.cpp
        src/Game.cpp
        src/NetworkManager.cpp
        src/ECS.cpp
    )
    
    # Set output name for web
    set_target_properties(${PROJECT_NAME}_web PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        SUFFIX ".html"
    )
    
    # Include directories for web target
    target_include_directories(${PROJECT_NAME}_web PRIVATE 
        include
        ${raylib_SOURCE_DIR}/src
        ${enet_SOURCE_DIR}/include
        ${entt_SOURCE_DIR}/src
        ${boost_SOURCE_DIR}
    )
    
    # Link libraries for web target
    target_link_libraries(${PROJECT_NAME}_web 
        raylib
        enet
        EnTT::EnTT
        Boost::system
        Boost::filesystem
        Boost::thread
        Boost::chrono
        Boost::date_time
        Boost::regex
        Boost::serialization
        Boost::program_options
    )
    
    # Emscripten-specific settings
    set_target_properties(${PROJECT_NAME}_web PROPERTIES
        LINK_FLAGS "-s USE_WEBGL2=1 -s FULL_ES3=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] -s EXPORTED_FUNCTIONS=['_main'] -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=33554432 -s MAXIMUM_MEMORY=268435456 --preload-file assets@assets"
    )
    
    # Copy assets for web build (in the same directory as the HTML file)
    file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${raylib_SOURCE_DIR}/src
    ${enet_SOURCE_DIR}/include
    ${entt_SOURCE_DIR}/src
    ${boost_SOURCE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    raylib
    enet
    EnTT::EnTT
    Boost::system
    Boost::filesystem
    Boost::thread
    Boost::chrono
    Boost::date_time
    Boost::regex
    Boost::serialization
    Boost::program_options
)

# Platform-specific settings
if(EMSCRIPTEN)
    # Emscripten-specific settings for web target
    target_compile_definitions(${PROJECT_NAME}_web PRIVATE
        __EMSCRIPTEN__
        PLATFORM_WEB
        GRAPHICS_API_OPENGL_ES2
    )
elseif(WIN32)
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
