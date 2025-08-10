#pragma once

#include "raylib.h"
#include <entt/entt.hpp>
#include <memory>

class NetworkManager;
class ECSSystem;

class Game {
public:
    Game();
    ~Game();
    
    void Initialize();
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
    
    void HandleInput();
    void UpdatePlayer();
};
