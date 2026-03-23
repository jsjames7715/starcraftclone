// Console System - Debug console
#pragma once
#include <string>
#include <vector>

struct ConsoleCommand {
    std::string name;
    std::string help;
};

class ConsoleSystem {
public:
    std::vector<ConsoleCommand> commands;
    std::string history[50];
    int historyCount;
    bool isOpen;
    
    void init();
    void toggle();
    void execute(const std::string& cmd);
    void addCommand(const std::string& name, const std::string& help);
};

void ConsoleSystem::init() {
    isOpen = false;
    historyCount = 0;
    addCommand("help", "Show commands");
    addCommand("quit", "Exit game");
    addCommand("restart", "Restart game");
    addCommand("god", "Toggle god mode");
    addCommand("kill", "Kill all enemies");
    addCommand("money", "Add 1000 minerals");
    addCommand("wave", "Spawn next wave");
    addCommand("fps", "Show FPS");
    addCommand("pos", "Show player position");
}

void ConsoleSystem::toggle() {
    isOpen = !isOpen;
}

void ConsoleSystem::execute(const std::string& cmd) {
    if (historyCount < 50) {
        history[historyCount++] = cmd;
    }
}

void ConsoleSystem::addCommand(const std::string& name, const std::string& help) {
    commands.push_back({name, help});
}
