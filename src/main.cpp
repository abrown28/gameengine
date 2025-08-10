#include "Game.h"
#include <iostream>

int main() {
    std::cout << "Starting Game Engine with Raylib and ENet..." << std::endl;
    
    Game game;
    
    try {
        game.Initialize();
        
        // Main game loop
        while (game.IsRunning()) {
            game.Update();
            game.Render();
        }
        
        game.Shutdown();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    std::cout << "Game Engine shutdown complete." << std::endl;
    return 0;
}
