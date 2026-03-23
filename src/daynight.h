// DayNight System - Day/night cycle
#pragma once

class DayNightSystem {
public:
    float timeOfDay;
    float dayLength;
    bool isNight;
    
    void init(float dayLen);
    void update(float dt);
    float getBrightness();
    float getVisionRangeModifier();
};

void DayNightSystem::init(float dayLen) {
    dayLength = dayLen;
    timeOfDay = 0;
    isNight = false;
}

void DayNightSystem::update(float dt) {
    timeOfDay += dt;
    if (timeOfDay >= dayLength) {
        timeOfDay = 0;
    }
    isNight = timeOfDay > dayLength * 0.6f || timeOfDay < dayLength * 0.1f;
}

float DayNightSystem::getBrightness() {
    float normalized = timeOfDay / dayLength;
    if (normalized < 0.1f) return 0.2f;
    if (normalized > 0.9f) return 0.2f;
    if (normalized < 0.2f || normalized > 0.8f) return 0.5f;
    return 1.0f;
}

float DayNightSystem::getVisionRangeModifier() {
    return isNight ? 0.6f : 1.0f;
}
