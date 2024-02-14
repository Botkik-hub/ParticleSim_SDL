#pragma once
#include <set>
#include "SDL_render.h"

enum class ParticleType;
class Particle;


class Field
{
    int m_width;
    int m_height;
    int m_size;

    bool m_frameToUpdateFlag;
    
    std::set<Particle*> m_activeParticles;
    Particle* m_particles;
    
    SDL_Renderer* m_renderer = nullptr;
    bool m_needUpdateTexture;
    Uint32* m_textureColors;
    SDL_Rect m_updateArea;
    SDL_Texture* m_texture;
public:
    Field();
    ~Field();

    void Update(float dt);
    void CopyTexturePart();
    void Render();

    int Ind(int x, int y) const;
    void Coord(int i, int& x, int& y) const;

    void UpdateTexture(int ind, Uint32 color);

    void SpawnParticle(int x, int y, ParticleType type);
    
private:
    static Uint32 ColToUint(SDL_Color color);

private:
    void UpdateParticle(Particle* particle, int x, int y);

    void UpdateSand(Particle* particle, int x, int y);
    void SwapParticles(int ind, int indOther);
    void UpdateWater(Particle* particle, int x, int y);

    Uint32 GetColorByType(ParticleType type);
};
