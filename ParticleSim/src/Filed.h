#pragma once
#include "SDL_render.h"


enum class ParticleType
{
    None,
    Sand,
    Water
};

struct Data
{
    ParticleType type = ParticleType::None;
    bool hasMoved;
};

class Filed
{
    ParticleType* m_particles = nullptr;
    //Data* m_particlesBuffer = nullptr;
    SDL_Renderer* m_renderer = nullptr;

    int m_width;
    int m_height;
    int m_size;
public:

    Filed();
    ~Filed();
    void ApplyGravity(int x, int y) const;
    void UpdateTile(int x, int y) const;

    void Update(float dt) const;
    void Render() const;

private:
    void SetColor(ParticleType type) const;

    void SwapParticles(int x1, int y1, int x2, int y2) const;

    void SwapParticles(int i1, int i2) const;
    
    bool CanSwap(int i1, int i2) const;
    
    int Ind(int x, int y) const;
};
