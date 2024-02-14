#include "Field.h"

#include <algorithm>

#include "Particle.h"
#include "TheGame.h"

Field::Field()
{
    m_width = TheGame::WIDTH;
    m_height = TheGame::HEIGHT;
    m_size = m_height * m_width;
    m_renderer = TheGame::Instance().GetRenderer();

    m_frameToUpdateFlag = true;
    m_textureColors = new Uint32[m_size];
    m_particles = new Particle[m_size];
    m_needUpdateTexture = false;

    for (int i = 0; i < m_size; ++i)
    {
        m_particles[i].type = ParticleType::None;
        m_textureColors[i] = ColToUint({0, 0, 0, 255});
    }
    
    
    m_texture = SDL_CreateTexture(m_renderer,  SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
}



Field::~Field()
{
    delete[] m_textureColors;
    delete[] m_particles;
    SDL_DestroyTexture(m_texture);
}

void Field::Update(float dt)
{
    m_frameToUpdateFlag = !m_frameToUpdateFlag;
    for (int i = m_size- 1; i >= 0; --i)
    {
        if (m_particles[i].type == ParticleType::None) continue;
        if (m_particles[i].frameToUpdateFlag == m_frameToUpdateFlag) continue;
        m_particles[i].frameToUpdateFlag = m_frameToUpdateFlag;
        
        int x, y;
        Coord(i, x, y);
        UpdateParticle(&m_particles[i], x, y);
    }
    for (int i =  0; i < m_size; ++i)
    {
        if (m_particles[i].type == ParticleType::None) continue;
        if (m_particles[i].frameToUpdateFlag == m_frameToUpdateFlag) continue;
        m_particles[i].frameToUpdateFlag = m_frameToUpdateFlag;
        
        int x, y;
        Coord(i, x, y);
        UpdateParticle(&m_particles[i], x, y);
    }

}


void Field::Render()
{
    if (m_needUpdateTexture)
    {
        //CopyTexturePart();
        SDL_UpdateTexture(m_texture, nullptr, m_textureColors, m_width * sizeof(Uint32));
        m_needUpdateTexture = false;
    }

    SDL_RenderCopy(m_renderer, m_texture , nullptr, nullptr);
}

Uint32 Field::ColToUint(const SDL_Color color)
{
    return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
}

int Field::Ind(const int x, const int y) const
{
    return x + y * m_width;
}

void Field::Coord(int i, int& x, int& y) const
{
    x = i % m_width;
    y = i / m_width;
}

void Field::UpdateTexture(int ind, Uint32 color)
{
    m_needUpdateTexture = true;

    m_textureColors[ind] = color;
}

void Field::SpawnParticle(int x, int y, ParticleType type)
{
    int ind = Ind(x,y);
    m_particles[ind].type= type;
}


void Field::UpdateParticle(Particle* particle, int x, int y)
{
    switch (particle->type)
    {
    case ParticleType::None:
        break;
    case ParticleType::Sand:
        UpdateSand(particle, x, y);
        break;
    case ParticleType::Water:
        UpdateWater(particle, x, y);
        break;
    }
}

void Field::UpdateSand(Particle* particle, int x, int y)
{
    if (y >= m_height) return;

    const int ind = Ind(x, y);
    const int indUnder = Ind(x, y + 1);
    
    if (m_particles[indUnder].type == ParticleType::None || m_particles[indUnder].type == ParticleType::Water )
    {
        SwapParticles(ind, indUnder);
        return;
    }
    const int indUnderLeft = Ind(x - 1, y + 1);
    if (x > 0 && m_particles[indUnderLeft].type == ParticleType::None || m_particles[indUnderLeft].type == ParticleType::Water )
    {
        SwapParticles(ind, indUnderLeft);
        return;
    }
    const int indUnderRight = Ind(x + 1, y + 1);
    if (x < m_width - 1 && m_particles[indUnderRight].type == ParticleType::None || m_particles[indUnderRight].type == ParticleType::Water )
    {
        SwapParticles(ind, indUnderRight);
        return;
    }
}

void Field::SwapParticles(const int ind, const int indOther)
{
    const Particle temp = m_particles[ind];
    m_particles[ind] = m_particles[indOther];
    m_particles[indOther] = temp;
    UpdateTexture(ind, GetColorByType(m_particles[ind].type));
    UpdateTexture(indOther, GetColorByType(m_particles[indOther].type));
}

void Field::UpdateWater(Particle* particle, int x, int y)
{
    const int ind = Ind(x, y);
    const int indUnder = Ind(x, y + 1);
    if (y < m_height - 1 && m_particles[indUnder].type == ParticleType::None)
    {
        SwapParticles(ind, indUnder);
        return;
    }
    const int indUnderLeft = Ind(x - 1, y + 1);
    if (y < m_height - 1 && x > 0 && m_particles[indUnderLeft].type == ParticleType::None)
    {
        SwapParticles(ind, indUnderLeft);
        return;
    }
    const int indUnderRight = Ind(x + 1, y + 1);
    if (y < m_height - 1 && x < m_width - 1 && m_particles[indUnderRight].type == ParticleType::None)
    {
        SwapParticles(ind, indUnderRight);
        return;
    }
    const int indLeft = Ind(x - 1, y);
    if (x > 0 && m_particles[indLeft].type == ParticleType::None)
    {
        SwapParticles(ind, indLeft);
        return;
    }
    const int indRight = Ind(x + 1, y);
    if (x < m_width - 1 && m_particles[indRight].type == ParticleType::None)
    {
        SwapParticles(ind, indRight);
        return;
    }
}

Uint32 Field::GetColorByType(const ParticleType type)
{
    switch (type) {
    case ParticleType::None:
        return 0;
        break;
    case ParticleType::Sand:
        return 255u << 24 | 255u << 16 | 0u << 8 | 255;
        break;
    case ParticleType::Water:
        return 0u << 24 | 0u << 16 | 255u << 8 | 255;
        break;
    }
    return 0;
}

void Field::CopyTexturePart()
{
    // SDL_UpdateTexture(m_texture, nullptr, m_textureColors, m_width * sizeof(Uint32))
    // // make array of colors by rect
    // Uint32* rectColors = new Uint32[m_updateArea.w * m_updateArea.h];
    // for (int y = 0; y < m_updateArea.h; ++y)
    // {
    //     Uint32* thisLineStart = m_textureColors + ((m_updateArea.y + y) * m_width + m_updateArea.x);
    //     Uint32* thisLineEnd = m_textureColors + ((m_updateArea.y + y) * m_width + m_updateArea.x + m_updateArea.w);
    //     Uint32* rectLineStart = rectColors + (y * m_updateArea.w);
    //     std::copy(thisLineStart, thisLineEnd, rectLineStart);
    // }
    //     
    // SDL_UpdateTexture(m_texture, &m_updateArea, rectColors, m_updateArea.w * sizeof(Uint32));
    // delete[] rectColors;
    // m_updateArea = {-1, -1, 0, 0};
}
