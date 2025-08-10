#pragma once

#include <enet/enet.h>
#include <string>
#include <vector>

struct NetworkMessage {
    std::string data;
    ENetPeer* peer;
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();
    
    bool Initialize();
    void Shutdown();
    
    // Server functions
    bool StartServer(int port = 12345);
    void StopServer();
    
    // Client functions
    bool ConnectToServer(const std::string& address, int port = 12345);
    void Disconnect();
    
    // Message handling
    void SendMessage(const std::string& message);
    std::vector<NetworkMessage> GetMessages();
    
    // Update function to be called each frame
    void Update();
    
    bool IsServer() const { return isServer; }
    bool IsConnected() const { return isConnected; }
    
private:
    ENetHost* host;
    ENetPeer* peer;
    bool isServer;
    bool isConnected;
    std::vector<NetworkMessage> messageQueue;
    
    void ProcessEvents();
};
