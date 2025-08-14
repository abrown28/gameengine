#include "Game.h"
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    // Set up command line options
    po::options_description desc("Game Engine Options");
    desc.add_options()
        ("help,h", "Show this help message")
        ("server,s", "Start in server mode")
        ("client,c", "Start in client mode (default)")
        ("port,p", po::value<int>()->default_value(12345), "Network port (default: 12345)")
        ("host", po::value<std::string>()->default_value("127.0.0.1"), "Server host for client mode (default: 127.0.0.1)")
        ("verbose,v", "Enable verbose output")
        ("fullscreen,f", "Start in fullscreen mode")
        ("width", po::value<int>()->default_value(800), "Window width (default: 800)")
        ("height", po::value<int>()->default_value(600), "Window height (default: 600)");
    
    po::variables_map vm;
    
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "Error parsing command line options: " << e.what() << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    
    // Show help if requested
    if (vm.count("help")) {
        std::cout << "Game Engine with Raylib, ENet, EnTT, and Boost" << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }
    
    // Parse command line options
    bool isServer = vm.count("server") > 0;
    int port = vm["port"].as<int>();
    std::string host = vm["host"].as<std::string>();
    bool verbose = vm.count("verbose") > 0;
    bool fullscreen = vm.count("fullscreen") > 0;
    int width = vm["width"].as<int>();
    int height = vm["height"].as<int>();
    
    if (verbose) {
        std::cout << "Command line options:" << std::endl;
        std::cout << "  Mode: " << (isServer ? "Server" : "Client") << std::endl;
        std::cout << "  Port: " << port << std::endl;
        if (!isServer) {
            std::cout << "  Host: " << host << std::endl;
        }
        std::cout << "  Window: " << width << "x" << height << (fullscreen ? " (fullscreen)" : "") << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "Starting Game Engine with Raylib, ENet, EnTT, and Boost..." << std::endl;
    std::cout << "Mode: " << (isServer ? "Server" : "Client") << std::endl;
    
    Game game;
    
    try {
        game.Initialize(isServer, port, host, width, height, fullscreen);
        
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
