#pragma once
#include <algorithm>
#include <vector>

#include "SDL_render.h"

enum class ParticleType;
struct Particle;


class Field
{
    int m_width;
    int m_height;
    int m_size;

    bool m_frameToUpdateFlag;
    
    Particle* m_particles;

    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_texture;

    Uint32* m_RawTexturePtr;
    bool m_isTextureLocked;
    int m_pitch;
    
public:
    Field();
    ~Field();
    void UpdateParticle(int x, int y);

    void Update(float dt);
    void Render();

    int Ind(int x, int y) const;
    void Cord(int i, int& x, int& y) const;

    void UpdateTexture(int ind, Uint32 color);

    void SpawnParticle(int x, int y, ParticleType type);
    
private:
    static Uint32 ColToUint(SDL_Color color);

private:

    void UpdateFall(int x, int y);
    void UpdateRaise(int x, int y);
    void UpdateSides(int x, int y);
    
    void SwapParticles(int ind, int indOther);
};
