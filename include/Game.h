#pragma once

#include "raylib.h"
#include <entt/entt.hpp>
#include <memory>

// Boost includes for demonstration
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>

class NetworkManager;
class ECSSystem;

class Game {
public:
    Game();
    ~Game();
    
    void Initialize(bool isServer = false, int port = 12345, const std::string& host = "127.0.0.1", 
                   int width = 800, int height = 600, bool fullscreen = false);
    void Update();
    void Render();
    void Shutdown();
    
    bool IsRunning() const { return running; }
    
private:
    bool running;
    std::unique_ptr<NetworkManager> networkManager;
    std::unique_ptr<ECSSystem> ecsSystem;
    
    // Game state
    Vector2 playerPosition;
    float playerSpeed;
    
    // Colors
    Color backgroundColor;
    Color playerColor;
    
    // Player entity
    entt::entity playerEntity;
    
    // Boost utilities for demonstration
    boost::filesystem::path assetsPath;
    boost::chrono::steady_clock::time_point lastBoostUpdate;
    boost::regex versionRegex;
    
    void HandleInput();
    void UpdatePlayer();
    void InitializeBoostFeatures();
    void UpdateBoostFeatures();
};
