#pragma once
#include <algorithm>
#include <vector>

#include "IVec2.h"
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

    void Update();
    void Render();

    int Ind(int x, int y) const;
    int Ind(const IVec2& cord) const;
    void Cord(int i, int& x, int& y) const;

    IVec2 Cord(int index) const;
    
    void UpdateTexture(int ind, Uint32 color);

    void SpawnParticle(int x, int y, ParticleType type);
    
private:
    static Uint32 ColToUint(SDL_Color color);

private:
    void UpdateParticle(int x, int y, int index);

    void UpdateVelocity(int index, IVec2 cord);

    void AddVerticalVelocity(int index, IVec2 cord);
    void AddHorizontalVelocity(int index, IVec2 cord);

    void UpdateMovement(int index, IVec2 cord);
    
    void UpdateFall(int x, int y, int index);
    void UpdateRaise(int x, int y, int index);
    void UpdateSides(int x, int y, int index);
    
    void SwapParticles(int ind, int indOther);

    bool IsSomethingUnder(IVec2 cord) const;
};
