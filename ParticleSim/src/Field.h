#pragma once
#include <vector>

#include "SDL_egl.h"
#include "SDL_render.h"


enum class ParticleType
{
    None,
    Sand,
    Water
};


struct Pixel
{
    int x;
    int y;
    Uint32 color;
};

class Field
{
    ParticleType* m_particles = nullptr;
    SDL_Renderer* m_renderer = nullptr;

    int m_width;
    int m_height;
    int m_size;

    bool m_needUpdateTexture;
    Uint32* m_textureColors;
    SDL_Rect m_updateArea;
    SDL_Texture* m_texture;
public:

    Field();
    ~Field();
    void ApplyGravity(int x, int y);
    void UpdateTile(int x, int y);

    void Update(float dt);
    void Render() const;

    int Ind(int x, int y) const;
    void Coord(int i, int& x, int& y) const;

    void UpdateTexture(const std::vector<Pixel>& pixels);
private:
    Uint32 GetColor(ParticleType type) const;

    //void SwapParticles(int x1, int y1, int x2, int y2);

    void SwapParticles(int i1, int i2);
    
    bool CanSwap(int i1, int i2) const;

    static Uint32 ColToUint(SDL_Color color);
};
