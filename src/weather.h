// Weather System - Dynamic weather effects
#pragma once
#include <string>

enum class WeatherType {
    Clear,
    Cloudy,
    Rain,
    Storm,
    Snow,
    Fog,
    Eclipse
};

class WeatherSystem {
public:
    WeatherType currentWeather;
    float intensity;
    float transitionSpeed;
    float timer;
    bool enabled;
    
    void init();
    void setWeather(WeatherType type, float intensity);
    void update(float dt);
    float getVisibilityModifier();
    float getMovementModifier();
};

void WeatherSystem::init() {
    currentWeather = WeatherType::Clear;
    intensity = 0;
    transitionSpeed = 0.1f;
    timer = 0;
    enabled = true;
}

void WeatherSystem::setWeather(WeatherType type, float intens) {
    currentWeather = type;
    intensity = intens;
}

void WeatherSystem::update(float dt) {
    timer += dt;
    if (timer > 60 && enabled) {
        int r = rand() % 10;
        if (r < 3) setWeather(WeatherType::Clear, 0);
        else if (r < 5) setWeather(WeatherType::Cloudy, 0.3f);
        else if (r < 7) setWeather(WeatherType::Rain, 0.5f);
        else if (r < 8) setWeather(WeatherType::Storm, 0.8f);
        else if (r < 9) setWeather(WeatherType::Fog, 0.6f);
        else setWeather(WeatherType::Eclipse, 0.4f);
        timer = 0;
    }
}

float WeatherSystem::getVisibilityModifier() {
    switch (currentWeather) {
        case WeatherType::Clear: return 1.0f;
        case WeatherType::Cloudy: return 0.9f;
        case WeatherType::Rain: return 0.8f;
        case WeatherType::Storm: return 0.6f;
        case WeatherType::Snow: return 0.7f;
        case WeatherType::Fog: return 0.4f;
        case WeatherType::Eclipse: return 0.5f;
    }
    return 1.0f;
}

float WeatherSystem::getMovementModifier() {
    switch (currentWeather) {
        case WeatherType::Clear: return 1.0f;
        case WeatherType::Cloudy: return 1.0f;
        case WeatherType::Rain: return 0.95f;
        case WeatherType::Storm: return 0.85f;
        case WeatherType::Snow: return 0.8f;
        case WeatherType::Fog: return 1.0f;
        case WeatherType::Eclipse: return 1.0f;
    }
    return 1.0f;
}
