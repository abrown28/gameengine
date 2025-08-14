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

void Game::Initialize(bool isServer, int port, const std::string& host, int width, int height, bool fullscreen) {
    // Initialize raylib
    InitWindow(width, height, "Game Engine - Raylib + ENet + EnTT");
    
    if (fullscreen) {
        ToggleFullscreen();
    }
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
    
    // Start networking based on command line options
    if(isServer) {
        std::cout << "Starting server on port " << port << std::endl;
        if (!networkManager->StartServer(port)) {
            std::cout << "Warning: Could not start server, running in offline mode" << std::endl;
        }
    } else {
        std::cout << "Connecting to server at " << host << ":" << port << std::endl;
        if (!networkManager->Connect(host.c_str(), port)) {
            std::cout << "Warning: Could not connect to server, running in offline mode" << std::endl;
        }
    }
    
    // Create player entity with 3D alien model
    playerEntity = ecsSystem->createEntity();
    ecsSystem->addComponent(playerEntity, ECSTransform{{400.0f, 300.0f}});
    ecsSystem->addComponent(playerEntity, Velocity{});
    ecsSystem->addComponent(playerEntity, Model3D{});
    ecsSystem->addComponent(playerEntity, Player{200.0f, "Player"});
    ecsSystem->addComponent(playerEntity, Networked{});
    
    // Set the player as the camera target
    ecsSystem->setCameraTarget(playerEntity);
    
    // Debug: Check which platform we're on
    #ifdef __EMSCRIPTEN__
        std::cout << "DEBUG: __EMSCRIPTEN__ is defined!" << std::endl;
    #else
        std::cout << "DEBUG: __EMSCRIPTEN__ is NOT defined!" << std::endl;
    #endif
    
    // Load the alien model
    #ifdef __EMSCRIPTEN__
        // In web builds, the path is relative to the preloaded assets root
        std::cout << "EMSCRIPTEN: Loading alien model from: Models/OBJ format/alien.obj" << std::endl;
        if (!ecsSystem->loadModel3D(playerEntity, "/assets/Models/OBJ format/alien.obj", 50.0f)) {
    #else
        // In desktop builds, use the full assets path
        std::cout << "DESKTOP: Loading alien model from: assets/Models/OBJ format/alien.obj" << std::endl;
        if (!ecsSystem->loadModel3D(playerEntity, "assets/Models/OBJ format/alien.obj", 50.0f)) {
    #endif
        std::cout << "Warning: Could not load alien model, falling back to 2D circle" << std::endl;
        // Fallback to 2D circle if model loading fails
        ecsSystem->removeComponent<Model3D>(playerEntity);
        ecsSystem->addComponent(playerEntity, Renderable{WHITE, 20.0f, true});
    }
    
    // Create some additional entities for demonstration - place them further out to show camera movement
    auto enemy1 = ecsSystem->createEntity();
    ecsSystem->addComponent(enemy1, ECSTransform{{200.0f, 200.0f}});
    ecsSystem->addComponent(enemy1, Velocity{{50.0f, 0.0f}});
    ecsSystem->addComponent(enemy1, Renderable{RED, 15.0f, true});
    
    auto enemy2 = ecsSystem->createEntity();
    ecsSystem->addComponent(enemy2, ECSTransform{{600.0f, 400.0f}});
    ecsSystem->addComponent(enemy2, Velocity{{-30.0f, 20.0f}});
    ecsSystem->addComponent(enemy2, Renderable{BLUE, 12.0f, true});
    
    // Add some static entities to show the world moving around the player
    auto static1 = ecsSystem->createEntity();
    ecsSystem->addComponent(static1, ECSTransform{{100.0f, 100.0f}});
    ecsSystem->addComponent(static1, Renderable{GREEN, 25.0f, true});
    
    auto static2 = ecsSystem->createEntity();
    ecsSystem->addComponent(static2, ECSTransform{{700.0f, 500.0f}});
    ecsSystem->addComponent(static2, Renderable{ORANGE, 30.0f, true});
    
    auto static3 = ecsSystem->createEntity();
    ecsSystem->addComponent(static3, ECSTransform{{100.0f, 500.0f}});
    ecsSystem->addComponent(static3, Renderable{PURPLE, 18.0f, true});
    
    auto static4 = ecsSystem->createEntity();
    ecsSystem->addComponent(static4, ECSTransform{{700.0f, 100.0f}});
    ecsSystem->addComponent(static4, Renderable{YELLOW, 22.0f, true});
    
    // Add more entities in a larger area to demonstrate camera movement
    for (int i = 0; i < 10; i++) {
        auto entity = ecsSystem->createEntity();
        float x = 100.0f + (i * 150.0f);
        float y = 50.0f + ((i % 3) * 200.0f);
        Color colors[] = {RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, LIME, SKYBLUE, GOLD};
        ecsSystem->addComponent(entity, ECSTransform{{x, y}});
        ecsSystem->addComponent(entity, Renderable{colors[i % 10], 15.0f + (i * 2.0f), true});
    }
    
    // Add some entities that move in patterns to show dynamic world
    for (int i = 0; i < 5; i++) {
        auto movingEntity = ecsSystem->createEntity();
        float x = 200.0f + (i * 120.0f);
        float y = 300.0f + (i * 50.0f);
        ecsSystem->addComponent(movingEntity, ECSTransform{{x, y}});
        ecsSystem->addComponent(movingEntity, Velocity{{30.0f + (i * 10.0f), -20.0f + (i * 5.0f)}});
        Color movingColor = {255, (unsigned char)(100 + i * 30), (unsigned char)(100 + i * 20), 255};
        ecsSystem->addComponent(movingEntity, Renderable{movingColor, 12.0f, true});
    }
    
    running = true;
    
    // Initialize Boost features
    InitializeBoostFeatures();
    
    std::cout << "Game initialized successfully with ECS, camera system, and Boost libraries!" << std::endl;
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
        ecsSystem->updateCamera(deltaTime);  // Update camera to follow player
        ecsSystem->updateNetworkSync();
    }
    
    // Update game logic
    UpdatePlayer();
    
    // Update Boost features
    UpdateBoostFeatures();
    
    // Check for window close
    if (WindowShouldClose()) {
        running = false;
    }
}

void Game::Render() {
    BeginDrawing();
    
    ClearBackground(backgroundColor);
    
    // Set up 3D camera for the scene - fixed position since entities are moved by ECS
    Camera3D camera = { 0 };
    Vector2 cameraOffset = ecsSystem->getCameraOffset();
    
    // 3D camera stays fixed - entities are moved by the ECS camera system
    //camera.position = { 400.0f, 200.0f, 200.0f };
    //camera.target = { 400.0f, 300.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    BeginMode3D(camera);
    
    // Draw a simple ground plane for reference - fixed position
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
    
    // Camera info
    DrawText(TextFormat("Camera Offset: (%.1f, %.1f)", cameraOffset.x, cameraOffset.y), 10, 60, 16, WHITE);
    
    // Draw a crosshair in the center to show the player is centered
    int centerX = GetScreenWidth() / 2;
    int centerY = GetScreenHeight() / 2;
    DrawLine(centerX - 20, centerY, centerX + 20, centerY, WHITE);
    DrawLine(centerX, centerY - 20, centerX, centerY + 20, WHITE);
    DrawCircle(centerX, centerY, 5, WHITE);
    
    // Instructions
    DrawText("WASD or Arrow Keys to move", 10, 150, 16, GRAY);
    DrawText("Player stays centered - world moves around you", 10, 170, 16, GRAY);
    
    // Network status
    if (networkManager) {
        if (networkManager->IsServer()) {
            DrawText("Server Mode", 10, 200, 16, GREEN);
        } else if (networkManager->IsConnected()) {
            DrawText("Client Mode - Connected", 10, 200, 16, GREEN);
        } else {
            DrawText("Offline Mode", 10, 200, 16, YELLOW);
        }
    }
    
    // ECS info
    if (ecsSystem) {
        auto entityCount = ecsSystem->getRegistry().storage<entt::entity>().size();
        DrawText(TextFormat("Entities: %zu", entityCount), 10, 230, 16, WHITE);
    }
    
    // Boost info
    DrawText("Boost Libraries: Filesystem, Thread, Chrono, Regex, Date/Time", 10, 250, 14, SKYBLUE);
    if (boost::filesystem::exists(assetsPath)) {
        DrawText(TextFormat("Assets Path: %s", assetsPath.string().c_str()), 10, 270, 12, GRAY);
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

void Game::InitializeBoostFeatures() {
    // Initialize Boost filesystem path
    assetsPath = boost::filesystem::current_path() / "assets";
    
    // Initialize Boost regex for version matching
    versionRegex = boost::regex("v\\d+\\.\\d+\\.\\d+");
    
    // Initialize Boost chrono timer
    lastBoostUpdate = boost::chrono::steady_clock::now();
    
    // Demonstrate Boost filesystem
    if (boost::filesystem::exists(assetsPath)) {
        std::cout << "Boost: Assets directory found at: " << assetsPath.string() << std::endl;
        
        // Count files in assets directory
        int fileCount = 0;
        for (const auto& entry : boost::filesystem::directory_iterator(assetsPath)) {
            if (boost::filesystem::is_regular_file(entry)) {
                fileCount++;
            }
        }
        std::cout << "Boost: Found " << fileCount << " files in assets directory" << std::endl;
    } else {
        std::cout << "Boost: Assets directory not found at: " << assetsPath.string() << std::endl;
    }
    
    // Demonstrate Boost date/time
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    std::cout << "Boost: Current time: " << boost::posix_time::to_simple_string(now) << std::endl;
}

void Game::UpdateBoostFeatures() {
    // Update Boost features every 5 seconds
    auto now = boost::chrono::steady_clock::now();
    auto elapsed = boost::chrono::duration_cast<boost::chrono::seconds>(now - lastBoostUpdate);
    
    if (elapsed.count() >= 5) {
        // Demonstrate Boost regex
        std::string testString = "Game version v1.2.3 is running";
        if (boost::regex_search(testString, versionRegex)) {
            std::cout << "Boost: Version pattern found in string" << std::endl;
        }
        
        // Demonstrate Boost filesystem file size
        if (boost::filesystem::exists(assetsPath)) {
            boost::filesystem::space_info space = boost::filesystem::space(assetsPath);
            std::cout << "Boost: Available space: " << space.available / (1024*1024) << " MB" << std::endl;
        }
        
        lastBoostUpdate = now;
    }
}
