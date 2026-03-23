// Network System - Multiplayer networking
#pragma once
#include <string>
#include <vector>

enum class PacketType {
    PLAYER_JOIN,
    PLAYER_LEAVE,
    PLAYER_STATE,
    CHAT_MESSAGE,
    GAME_START,
    GAME_END
};

struct NetworkPlayer {
    int id;
    std::string name;
    int team;
    bool ready;
};

class NetworkSystem {
public:
    bool isHost;
    bool isConnected;
    int localPlayerId;
    std::vector<NetworkPlayer> players;
    
    bool host(int port);
    bool connect(const std::string& address, int port);
    void disconnect();
    void sendChat(const std::string& message);
};

bool NetworkSystem::host(int port) {
    isHost = true;
    isConnected = true;
    return true;
}

bool NetworkSystem::connect(const std::string& address, int port) {
    isHost = false;
    isConnected = true;
    return true;
}

void NetworkSystem::disconnect() {
    isConnected = false;
    players.clear();
}

void NetworkSystem::sendChat(const std::string& message) {
    // Send chat packet to server
}
