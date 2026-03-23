// Minimap System - Strategic overview
#pragma once

struct MinimapMarker {
    int x, y;
    int color;
};

class MinimapSystem {
public:
    int width, height;
    int mapWidth, mapHeight;
    MinimapMarker markers[100];
    int markerCount;
    
    void init(int w, int h, int mw, int mh);
    void addMarker(int x, int y, int color);
    void removeMarker(int x, int y);
    void clearMarkers();
    void worldToMinimap(int wx, int wy, int& mx, int& my);
    void minimapToWorld(int mx, int my, int& wx, int& wy);
};

void MinimapSystem::init(int w, int h, int mw, int mh) {
    width = w;
    height = h;
    mapWidth = mw;
    mapHeight = mh;
    markerCount = 0;
}

void MinimapSystem::addMarker(int x, int y, int color) {
    if (markerCount < 100) {
        markers[markerCount].x = x;
        markers[markerCount].y = y;
        markers[markerCount].color = color;
        markerCount++;
    }
}

void MinimapSystem::removeMarker(int x, int y) {
    for (int i = 0; i < markerCount; i++) {
        if (markers[i].x == x && markers[i].y == y) {
            markers[i] = markers[markerCount - 1];
            markerCount--;
            break;
        }
    }
}

void MinimapSystem::clearMarkers() {
    markerCount = 0;
}

void MinimapSystem::worldToMinimap(int wx, int wy, int& mx, int& my) {
    mx = (wx * width) / mapWidth;
    my = (wy * height) / mapHeight;
}

void MinimapSystem::minimapToWorld(int mx, int my, int& wx, int& wy) {
    wx = (mx * mapWidth) / width;
    wy = (my * mapHeight) / height;
}
