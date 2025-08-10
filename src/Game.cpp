#include "Game.h"
#include "NetworkManager.h"
#include "ECS.h"
#include <iostream>

Game::Game() 
    : running(false)
    , playerPosition({400.0f, 300.0f})
    , playerSpeed(200.0f)
    , backgroundColor({25, 25, 35, 255})
    , playerColor({255, 255, 255, 255}) {
}

Game::~Game() {
    Shutdown();
}

void Game::Initialize() {
    // Initialize raylib
    InitWindow(800, 600, "Game Engine - Raylib + ENet + EnTT");
    SetTargetFPS(60);
    
    // Initialize 3D camera
    InitAudioDevice();
    
    // Initialize ECS system
    ecsSystem = std::make_unique<ECSSystem>();
    
    // Initialize network manager
    networkManager = std::make_unique<NetworkManager>();
    if (!networkManager->Initialize()) {
        throw std::runtime_error("Failed to initialize network manager");
    }
    
    // For demo purposes, start as a server
    if (!networkManager->StartServer(12345)) {
        std::cout << "Warning: Could not start server, running in offline mode" << std::endl;
    }
    
    // Create player entity with 3D alien model
    playerEntity = ecsSystem->createEntity();
    ecsSystem->addComponent(playerEntity, ECSTransform{{400.0f, 300.0f}});
    ecsSystem->addComponent(playerEntity, Velocity{});
    ecsSystem->addComponent(playerEntity, Model3D{});
    ecsSystem->addComponent(playerEntity, Player{200.0f, "Player"});
    ecsSystem->addComponent(playerEntity, Networked{});
    
    // Load the alien model
    if (!ecsSystem->loadModel3D(playerEntity, "assets/Models/OBJ format/alien.obj", 50.0f)) {
        std::cout << "Warning: Could not load alien model, falling back to 2D circle" << std::endl;
        // Fallback to 2D circle if model loading fails
        ecsSystem->removeComponent<Model3D>(playerEntity);
        ecsSystem->addComponent(playerEntity, Renderable{WHITE, 20.0f, true});
    }
    
    // Create some additional entities for demonstration
    auto enemy1 = ecsSystem->createEntity();
    ecsSystem->addComponent(enemy1, ECSTransform{{200.0f, 200.0f}});
    ecsSystem->addComponent(enemy1, Velocity{{50.0f, 0.0f}});
    ecsSystem->addComponent(enemy1, Renderable{RED, 15.0f, true});
    
    auto enemy2 = ecsSystem->createEntity();
    ecsSystem->addComponent(enemy2, ECSTransform{{600.0f, 400.0f}});
    ecsSystem->addComponent(enemy2, Velocity{{-30.0f, 20.0f}});
    ecsSystem->addComponent(enemy2, Renderable{BLUE, 12.0f, true});
    
    running = true;
    std::cout << "Game initialized successfully with ECS!" << std::endl;
}

void Game::Update() {
    if (!running) return;
    
    float deltaTime = GetFrameTime();
    
    // Update network
    if (networkManager) {
        networkManager->Update();
    }
    
    // Handle input
    HandleInput();
    
    // Update ECS systems
    if (ecsSystem) {
        ecsSystem->updateMovement(deltaTime);
        ecsSystem->updateNetworkSync();
    }
    
    // Update game logic
    UpdatePlayer();
    
    // Check for window close
    if (WindowShouldClose()) {
        running = false;
    }
}

void Game::Render() {
    BeginDrawing();
    
    ClearBackground(backgroundColor);
    
    // Set up 3D camera for the scene
    Camera3D camera = { 0 };
    camera.position = { 400.0f, 200.0f, 200.0f };
    camera.target = { 400.0f, 300.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    BeginMode3D(camera);
    
    // Draw a simple ground plane for reference
    DrawPlane({ 400.0f, 300.0f, 0.0f }, { 800.0f, 600.0f }, LIGHTGRAY);
    
    // Render all entities using ECS
    if (ecsSystem) {
        ecsSystem->updateRendering();
    }
    
    EndMode3D();
    
    // Draw UI
    DrawText("Game Engine - Raylib + ENet + EnTT", 10, 10, 20, WHITE);
    
    // Get player position from ECS
    if (ecsSystem && ecsSystem->hasComponent<ECSTransform>(playerEntity)) {
        auto& transform = ecsSystem->getComponent<ECSTransform>(playerEntity);
        DrawText(TextFormat("Player Position: (%.1f, %.1f)", transform.position.x, transform.position.y), 10, 40, 16, WHITE);
    }
    
    // Network status
    if (networkManager) {
        if (networkManager->IsServer()) {
            DrawText("Server Mode", 10, 70, 16, GREEN);
        } else if (networkManager->IsConnected()) {
            DrawText("Client Mode - Connected", 10, 70, 16, GREEN);
        } else {
            DrawText("Offline Mode", 10, 70, 16, YELLOW);
        }
    }
    
    // ECS info
    if (ecsSystem) {
        auto entityCount = ecsSystem->getRegistry().size();
        DrawText(TextFormat("Entities: %zu", entityCount), 10, 100, 16, WHITE);
    }
    
    DrawText("Press ESC to exit", 10, 570, 16, GRAY);
    
    EndDrawing();
}

void Game::Shutdown() {
    if (networkManager) {
        networkManager->Shutdown();
    }
    
    CloseWindow();
    running = false;
}

void Game::HandleInput() {
    if (!ecsSystem || !ecsSystem->hasComponent<ECSTransform>(playerEntity) || !ecsSystem->hasComponent<Velocity>(playerEntity) || !ecsSystem->hasComponent<Player>(playerEntity)) {
        return;
    }
    
    auto& transform = ecsSystem->getComponent<ECSTransform>(playerEntity);
    auto& velocity = ecsSystem->getComponent<Velocity>(playerEntity);
    auto& player = ecsSystem->getComponent<Player>(playerEntity);
    
    // Player movement
    Vector2 movement = {0.0f, 0.0f};
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        movement.y -= 1.0f;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        movement.y += 1.0f; 
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        movement.x -= 1.0f;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        movement.x += 1.0f;
    }
    
    // Normalize movement vector
    if (movement.x != 0.0f || movement.y != 0.0f) {
        float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }
    
    // Update velocity based on input
    velocity.linear.x = movement.x * player.speed;
    velocity.linear.y = movement.y * player.speed;
}

void Game::UpdatePlayer() {
    if (!ecsSystem || !ecsSystem->hasComponent<ECSTransform>(playerEntity) || !ecsSystem->hasComponent<Networked>(playerEntity)) {
        return;
    }
    
    auto& transform = ecsSystem->getComponent<ECSTransform>(playerEntity);
    auto& networked = ecsSystem->getComponent<Networked>(playerEntity);
    
    // Send player position to network if connected
    if (networkManager && networkManager->IsConnected() && networked.needsSync) {
        networkManager->SendMessage(networked.lastSyncData);
        networked.needsSync = false;
    }
    
    // Process incoming network messages
    if (networkManager) {
        auto messages = networkManager->GetMessages();
        for (const auto& msg : messages) {
            // Handle different message types here
            if (msg.data.substr(0, 4) == "POS:") {
                // Handle position updates from other players
                // This is a simple example - in a real game you'd parse the position
                std::cout << "Received position update: " << msg.data << std::endl;
            }
        }
    }
}
