#include "ECS.h"
#include <iostream>

entt::entity ECSSystem::createEntity() {
    return registry.create();
}

void ECSSystem::destroyEntity(entt::entity entity) {
    registry.destroy(entity);
}

void ECSSystem::updateMovement(float deltaTime) {
    // Update entities with ECSTransform and Velocity components
    auto view = registry.view<ECSTransform, Velocity>();
    
    for (auto entity : view) {
        auto& transform = view.get<ECSTransform>(entity);
        auto& velocity = view.get<Velocity>(entity);
        
        // Update position
        transform.position.x += velocity.linear.x * deltaTime;
        transform.position.y += velocity.linear.y * deltaTime;
        
        // Update rotation
        transform.rotation += velocity.angular * deltaTime;
        
        // Keep entities in bounds (simple boundary checking)
        if (transform.position.x < 20.0f) transform.position.x = 20.0f;
        if (transform.position.x > 780.0f) transform.position.x = 780.0f;
        if (transform.position.y < 20.0f) transform.position.y = 20.0f;
        if (transform.position.y > 580.0f) transform.position.y = 580.0f;
    }
}

void ECSSystem::updateRendering() {
    // Render entities with ECSTransform and Renderable components (2D)
    auto view2D = registry.view<ECSTransform, Renderable>();
    
    for (auto entity : view2D) {
        const auto& transform = view2D.get<ECSTransform>(entity);
        const auto& renderable = view2D.get<Renderable>(entity);
        
        if (renderable.isCircle) {
            DrawCircleV(transform.position, renderable.radius, renderable.color);
        } else {
            Rectangle rect = renderable.rect;
            rect.x = transform.position.x - rect.width / 2;
            rect.y = transform.position.y - rect.height / 2;
            DrawRectangleRec(rect, renderable.color);
        }
    }
    
    // Render entities with ECSTransform and Model3D components (3D)
    auto view3D = registry.view<ECSTransform, Model3D>();
    
    for (auto entity : view3D) {
        const auto& transform = view3D.get<ECSTransform>(entity);
        const auto& model3D = view3D.get<Model3D>(entity);
        
        if (model3D.isLoaded) {
            // Draw the 3D model
            Vector3 modelPos = { transform.position.x, transform.position.y, 0.0f };
            DrawModel(model3D.model, modelPos, model3D.scale, WHITE);
            
            // Draw wireframe bounding box for debugging
            BoundingBox bbox = GetModelBoundingBox(model3D.model);
            DrawBoundingBox(bbox, RED);
        } else {
            // If model not loaded, draw a placeholder
            DrawSphere({ transform.position.x, transform.position.y, 0.0f }, 10.0f, YELLOW);
        }
    }
    
    // Render entities with ECSTransform and Alien3D components (3D Alien)
    auto viewAlien = registry.view<ECSTransform, Alien3D>();
    
    for (auto entity : viewAlien) {
        const auto& transform = viewAlien.get<ECSTransform>(entity);
        const auto& alien = viewAlien.get<Alien3D>(entity);
        
        // Draw a simple 3D alien using raylib shapes
        Vector3 alienPos = { transform.position.x, transform.position.y, 0.0f };
        
        // Draw alien body (sphere)
        DrawSphere(alienPos, alien.size * 15.0f, alien.color);
        
        // Draw alien head (smaller sphere on top)
        Vector3 headPos = { alienPos.x, alienPos.y - alien.size * 20.0f, alienPos.z };
        DrawSphere(headPos, alien.size * 8.0f, alien.color);
        
        // Draw alien eyes (two small spheres)
        Vector3 leftEye = { headPos.x - 3.0f, headPos.y - 2.0f, headPos.z + 5.0f };
        Vector3 rightEye = { headPos.x + 3.0f, headPos.y - 2.0f, headPos.z + 5.0f };
        DrawSphere(leftEye, 2.0f, BLACK);
        DrawSphere(rightEye, 2.0f, BLACK);
        
        // Draw alien antennae (two cylinders)
        Vector3 leftAntenna = { headPos.x - 4.0f, headPos.y - 8.0f, headPos.z };
        Vector3 rightAntenna = { headPos.x + 4.0f, headPos.y - 8.0f, headPos.z };
        DrawCylinder(leftAntenna, 1.0f, 1.0f, 8.0f, 8, alien.color);
        DrawCylinder(rightAntenna, 1.0f, 1.0f, 8.0f, 8, alien.color);
    }
}

void ECSSystem::updateNetworkSync() {
    // Update networked entities
    auto view = registry.view<ECSTransform, Networked>();
    
    for (auto entity : view) {
        auto& transform = view.get<ECSTransform>(entity);
        auto& networked = view.get<Networked>(entity);
        
        // Mark for network sync if position changed significantly
        networked.needsSync = true;
        networked.lastSyncData = "POS:" + std::to_string(transform.position.x) + "," + std::to_string(transform.position.y);
    }
}

bool ECSSystem::loadModel3D(entt::entity entity, const std::string& modelPath, float scale) {
    if (!registry.all_of<Model3D>(entity)) {
        std::cerr << "Entity does not have Model3D component" << std::endl;
        return false;
    }
    
    auto& model3D = registry.get<Model3D>(entity);
    
    // Load the model
    Model model = LoadModel(modelPath.c_str());
    
    if (model.meshCount == 0) {
        std::cerr << "Failed to load model: " << modelPath << std::endl;
        return false;
    }
    
    model3D.model = model;
    model3D.isLoaded = true;
    model3D.modelPath = modelPath;
    model3D.scale = scale;
    
    std::cout << "Successfully loaded model: " << modelPath << std::endl;
    std::cout << "Model info - Mesh count: " << model.meshCount << ", Scale: " << scale << std::endl;
    
    // Get and print bounding box info
    BoundingBox bbox = GetModelBoundingBox(model);
    std::cout << "Bounding box - Min: (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z 
              << ") Max: (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")" << std::endl;
    
    return true;
}
