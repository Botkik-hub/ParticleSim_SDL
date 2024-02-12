#pragma once
#include "SDL_render.h"


enum class ParticleType
{
    None,
    Sand,
    Water
};


class Filed
{
    ParticleType** m_particles = nullptr;
    //ParticleType** m_particlesBuffer = nullptr;
    SDL_Renderer* m_renderer = nullptr;
public:

    Filed();
    ~Filed();
    
    void Update(float dt);
    void Render() const;

private:
    void SetColor(ParticleType type) const;

    void SwapParticles(int x1, int y1, int x2, int y2);
};
