// Chat System - Multiplayer chat
#pragma once
#include <string>
#include <vector>

struct ChatMessage {
    std::string sender;
    std::string message;
    int timestamp;
};

class ChatSystem {
public:
    ChatMessage messages[100];
    int messageCount;
    int maxMessages;
    
    void init(int max);
    void sendMessage(const std::string& sender, const std::string& msg);
    void clear();
};

void ChatSystem::init(int max) {
    maxMessages = max;
    messageCount = 0;
}

void ChatSystem::sendMessage(const std::string& sender, const std::string& msg) {
    if (messageCount < maxMessages) {
        messages[messageCount].sender = sender;
        messages[messageCount].message = msg;
        messages[messageCount].timestamp = 0;
        messageCount++;
    } else {
        for (int i = 0; i < maxMessages - 1; i++) {
            messages[i] = messages[i + 1];
        }
        messages[maxMessages - 1].sender = sender;
        messages[maxMessages - 1].message = msg;
        messages[maxMessages - 1].timestamp = 0;
    }
}

void ChatSystem::clear() {
    messageCount = 0;
}
