#include "NetworkManager.h"
#include <iostream>
#include <cstring>

NetworkManager::NetworkManager() 
    : host(nullptr)
    , peer(nullptr)
    , isServer(false)
    , isConnected(false) {
}

NetworkManager::~NetworkManager() {
    Shutdown();
}

bool NetworkManager::Initialize() {
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet" << std::endl;
        return false;
    }
    
    std::cout << "ENet initialized successfully" << std::endl;
    return true;
}

void NetworkManager::Shutdown() {
    if (isConnected) {
        Disconnect();
    }
    
    if (isServer) {
        StopServer();
    }
    
    enet_deinitialize();
    std::cout << "ENet shutdown complete" << std::endl;
}

bool NetworkManager::StartServer(int port) {
    if (isServer || isConnected) {
        std::cerr << "Already running as server or connected as client" << std::endl;
        return false;
    }
    
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    
    host = enet_host_create(&address, 32, 2, 0, 0);
    if (host == nullptr) {
        std::cerr << "Failed to create server on port " << port << std::endl;
        return false;
    }
    
    isServer = true;
    std::cout << "Server started on port " << port << std::endl;
    return true;
}

void NetworkManager::StopServer() {
    if (!isServer || !host) return;
    
    enet_host_destroy(host);
    host = nullptr;
    isServer = false;
    std::cout << "Server stopped" << std::endl;
}

bool NetworkManager::ConnectToServer(const std::string& address, int port) {
    if (isServer || isConnected) {
        std::cerr << "Already running as server or connected as client" << std::endl;
        return false;
    }
    
    host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (host == nullptr) {
        std::cerr << "Failed to create client host" << std::endl;
        return false;
    }
    
    ENetAddress serverAddress;
    enet_address_set_host(&serverAddress, address.c_str());
    serverAddress.port = port;
    
    peer = enet_host_connect(host, &serverAddress, 2, 0);
    if (peer == nullptr) {
        std::cerr << "Failed to initiate connection to " << address << ":" << port << std::endl;
        enet_host_destroy(host);
        host = nullptr;
        return false;
    }
    
    std::cout << "Connecting to " << address << ":" << port << "..." << std::endl;
    return true;
}

void NetworkManager::Disconnect() {
    if (!isConnected || !peer) return;
    
    enet_peer_disconnect(peer, 0);
    
    // Wait for disconnect event
    ENetEvent event;
    while (enet_host_service(host, &event, 3000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected from server" << std::endl;
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            default:
                break;
        }
    }
    
    enet_peer_reset(peer);
    enet_host_destroy(host);
    host = nullptr;
    peer = nullptr;
    isConnected = false;
}

void NetworkManager::SendMessage(const std::string& message) {
    if (!isConnected && !isServer) return;
    
    ENetPacket* packet = enet_packet_create(
        message.c_str(),
        message.length() + 1,
        ENET_PACKET_FLAG_RELIABLE
    );
    
    if (isServer) {
        // Broadcast to all connected peers
        enet_host_broadcast(host, 0, packet);
    } else if (peer) {
        // Send to server
        enet_peer_send(peer, 0, packet);
    }
}

std::vector<NetworkMessage> NetworkManager::GetMessages() {
    std::vector<NetworkMessage> messages;
    std::swap(messages, messageQueue);
    return messages;
}

void NetworkManager::Update() {
    if (!host) return;
    
    ProcessEvents();
}

void NetworkManager::ProcessEvents() {
    ENetEvent event;
    
    while (enet_host_service(host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                if (isServer) {
                    std::cout << "Client connected from " 
                              << event.peer->address.host << ":" 
                              << event.peer->address.port << std::endl;
                } else {
                    std::cout << "Connected to server" << std::endl;
                    isConnected = true;
                }
                break;
            }
            
            case ENET_EVENT_TYPE_DISCONNECT: {
                if (isServer) {
                    std::cout << "Client disconnected" << std::endl;
                } else {
                    std::cout << "Disconnected from server" << std::endl;
                    isConnected = false;
                }
                event.peer->data = nullptr;
                break;
            }
            
            case ENET_EVENT_TYPE_RECEIVE: {
                NetworkMessage msg;
                msg.data = std::string((char*)event.packet->data);
                msg.peer = event.peer;
                messageQueue.push_back(msg);
                enet_packet_destroy(event.packet);
                break;
            }
            
            default:
                break;
        }
    }
}
