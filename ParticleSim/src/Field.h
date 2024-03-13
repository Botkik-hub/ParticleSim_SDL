#pragma once

#include <vector>

#include "IVec2.h"
#include "SDL_render.h"

#define ACTIVE_PARTICLES_DEBUG_VIEW false;


enum class ParticleType;
struct Particle;

class Field
{
    int m_width;
    int m_height;
    int m_size;

    Particle** m_particlesGrid;

    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_texture;

    Uint32* m_RawTexturePtr;
    bool m_isTextureLocked;
    int m_pitch;

    std::vector<Particle> m_particles;

   
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

    Particle* GetParticleAtPosition(IVec2 position) const;
    
private:
    static Uint32 ColToUint(SDL_Color color);

private:
    void UpdateParticle(Particle& particle);

    static void AddVerticalVelocity(Particle& particle);
    void AddHorizontalVelocity(Particle& particle) const;

    void MoveParticle(Particle& particle);
    
    void SwapParticles(int ind, int indOther);

    bool CanSwapParticles(const Particle& particle, const IVec2& direction) const;


private:
    #if ACTIVE_PARTICLES_DEBUG_VIEW
        // ReSharper disable once CppInconsistentNaming
        SDL_Texture* d_debugActiveTexture;
        // ReSharper disable once CppInconsistentNaming
        Uint32* d_debugRawTexturePointer;
    #endif
};

// todo this is plan how to organize particles 
// have all particles in array and a field with links to the particles
// have a linked list of particles that can be updated
