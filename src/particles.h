// Particle System - Visual effects
#pragma once

struct Particle {
    float x, y;
    float vx, vy;
    float life;
    int color;
};

class ParticleSystem {
public:
    Particle particles[1000];
    int count;
    
    void init();
    void emit(float x, float y, int type, int count);
    void update(float dt);
    void clear();
};

void ParticleSystem::init() {
    count = 0;
}

void ParticleSystem::emit(float x, float y, int type, int cnt) {
    for (int i = 0; i < cnt && count < 1000; i++) {
        particles[count].x = x;
        particles[count].y = y;
        particles[count].vx = (rand() % 100 - 50) / 10.0f;
        particles[count].vy = (rand() % 100) / 10.0f;
        particles[count].life = 1.0f;
        particles[count].color = type;
        count++;
    }
}

void ParticleSystem::update(float dt) {
    for (int i = 0; i < count; i++) {
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
        particles[i].life -= dt;
        if (particles[i].life <= 0) {
            particles[i] = particles[count - 1];
            count--;
        }
    }
}

void ParticleSystem::clear() {
    count = 0;
}
