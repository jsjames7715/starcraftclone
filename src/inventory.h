// Inventory System - Item management
#pragma once
#include <vector>
#include <string>

struct Item {
    int id;
    std::string name;
    std::string description;
    int quantity;
};

class InventorySystem {
public:
    std::vector<Item> items;
    int capacity;
    int gold;
    
    void init(int cap);
    bool addItem(const Item& item);
    bool removeItem(int id, int qty);
    Item* getItem(int id);
    int getTotalItems();
};

void InventorySystem::init(int cap) {
    capacity = cap;
    gold = 0;
}

bool InventorySystem::addItem(const Item& item) {
    for (auto& i : items) {
        if (i.id == item.id) {
            i.quantity += item.quantity;
            return true;
        }
    }
    if ((int)items.size() < capacity) {
        items.push_back(item);
        return true;
    }
    return false;
}

bool InventorySystem::removeItem(int id, int qty) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->id == id) {
            it->quantity -= qty;
            if (it->quantity <= 0) {
                items.erase(it);
            }
            return true;
        }
    }
    return false;
}

Item* InventorySystem::getItem(int id) {
    for (auto& i : items) {
        if (i.id == id) return &i;
    }
    return nullptr;
}

int InventorySystem::getTotalItems() {
    int total = 0;
    for (const auto& i : items) {
        total += i.quantity;
    }
    return total;
}
