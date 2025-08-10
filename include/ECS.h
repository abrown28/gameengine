#pragma once

#include <entt/entt.hpp>
#include "raylib.h"
#include <string>
#include <vector>

// Component definitions
struct ECSTransform {
    Vector2 position = {0.0f, 0.0f};
    float rotation = 0.0f;
    Vector2 scale = {1.0f, 1.0f};
};

struct Velocity {
    Vector2 linear = {0.0f, 0.0f};
    float angular = 0.0f;
};

struct Renderable {
    Color color = WHITE;
    float radius = 20.0f;  // For circles
    bool isCircle = true;
    Rectangle rect = {0, 0, 40, 40};  // For rectangles
};

struct Model3D {
    Model model;
    bool isLoaded = false;
    std::string modelPath = "";
    float scale = 1.0f;
};

struct Alien3D {
    bool isAlien = true;
    Color color = GREEN;
    float size = 1.0f;
};

struct Player {
    float speed = 200.0f;
    std::string name = "Player";
};

struct Networked {
    bool needsSync = false;
    std::string lastSyncData = "";
};

// ECS System class
class ECSSystem {
public:
    ECSSystem() = default;
    ~ECSSystem() = default;

    // Entity management
    entt::entity createEntity();
    void destroyEntity(entt::entity entity);
    
    // Component management
    template<typename Component>
    void addComponent(entt::entity entity, const Component& component);
    
    template<typename Component>
    Component& getComponent(entt::entity entity);
    
    template<typename Component>
    bool hasComponent(entt::entity entity);
    
    template<typename Component>
    void removeComponent(entt::entity entity);

    // System updates
    void updateMovement(float deltaTime);
    void updateRendering();
    void updateNetworkSync();
    
    // Model loading
    bool loadModel3D(entt::entity entity, const std::string& modelPath, float scale = 1.0f);

    // Query helpers
    template<typename... Components>
    auto view();

    // Registry access
    entt::registry& getRegistry() { return registry; }

private:
    entt::registry registry;
};

// Template implementations
template<typename Component>
void ECSSystem::addComponent(entt::entity entity, const Component& component) {
    registry.emplace<Component>(entity, component);
}

template<typename Component>
Component& ECSSystem::getComponent(entt::entity entity) {
    return registry.get<Component>(entity);
}

template<typename Component>
bool ECSSystem::hasComponent(entt::entity entity) {
    return registry.all_of<Component>(entity);
}

template<typename Component>
void ECSSystem::removeComponent(entt::entity entity) {
    registry.remove<Component>(entity);
}

template<typename... Components>
auto ECSSystem::view() {
    return registry.view<Components...>();
}
