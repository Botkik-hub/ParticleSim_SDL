#include "Filed.h"

#include <algorithm>

#include "TheGame.h"

Filed::Filed()
{
    m_width = TheGame::WIDTH;
    m_height = TheGame::HEIGHT;
    m_size = m_height * m_width;
    
    m_particles = new ParticleType[m_size];
    //m_particlesBuffer = new Data[m_size];
    m_renderer = TheGame::Instance().GetRenderer();

    for (int i = 0; i < m_size; ++i)
    {
        m_particles[i] = ParticleType::None;
    }
    // m_particles[Ind(300 ,300)] = ParticleType::Sand;
    
    for (int x = 170; x < 200; ++x)
    {
        for (int y = 50; y < 100; ++y)
        {
            m_particles[Ind(x,y)] = ParticleType::Sand;
        }
    }
    
    for (int x = 200; x < m_width; ++x)
    {
        for (int y = 50; y < 100; ++y)
        {
            m_particles[Ind(x,y)] = ParticleType::Water;
        }
    }
}

Filed::~Filed()
{
    delete m_particles;
}

void Filed::ApplyGravity(int x, int y) const
{
    if (y + 1 == m_height)
    {
        return;
    }

    const int current = Ind(x, y);
    const int bottom = Ind(x, y + 1);
    const int bottomRight = Ind(x + 1, y +1);
    const int bottomLeft = Ind(x - 1, y +1);
    
    if (CanSwap(current, bottom))
    {
        // m_particles[bottom] = m_particles[Ind(x,y)];
        // m_particles[current] = ParticleType::None;
        SwapParticles(current,bottom);
    }
    else if (x - 1 > 0 && CanSwap(current, bottomLeft))
    //else if (x - 1 > 0 && m_particles[bottomLeft] == ParticleType::None)
    {
        // m_particles[bottomLeft] = m_particles[current];
        // m_particles[current] = ParticleType::None;
        SwapParticles(current,bottomLeft);
    }
    else if (x + 1 < m_width && CanSwap(current, bottomRight))
    // else if (x + 1 < m_width && m_particles[bottomRight] == ParticleType::None)
    {
        SwapParticles(current, bottomRight);
        // m_particles[bottomRight] = m_particles[current];
        // m_particles[current] = ParticleType::None;
    }
    else
    {
        //m_particles[Ind(x,y)]= m_particles[Ind(x,y)];
    }
}

void Filed::UpdateTile(int x, int y) const
{
    ApplyGravity(x, y);
}

void Filed::Update(float dt) const
{
    for (int x = m_width - 1; x >= 0; --x)
    {
        for (int y = m_height - 1; y >= 0; --y)
        {
            if (m_particles[Ind(x,y)] != ParticleType::None)
            {
                UpdateTile(x, y);
            }
        }
    }

    // Data* end = m_particlesBuffer + (m_width * m_height + 1);
    // std::copy(m_particlesBuffer, end, m_particles);
    // for (int x = 0; x < m_width; ++x)
    // {
    //     for (int y = 0; y < m_height; ++y)
    //     {
    //         const int i = Ind(x,y);
    //         m_particles[i] = m_particlesBuffer[i];
    //         m_particlesBuffer[i] = ParticleType::None;
    //     }
    // }
}

void Filed::Render() const
{
    ParticleType lastParticle = ParticleType::None;
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < m_height; ++y)
        {
            const ParticleType current = m_particles[Ind(x,y)];
            if (current == ParticleType::None) continue;
            if (lastParticle != current)
            {
                SetColor(current);
                lastParticle = current;
            }
            SDL_RenderDrawPoint(m_renderer, x, y);
        }
    }
}

void Filed::SetColor(const ParticleType type) const
{
    SDL_Color color;
    switch (type)  // NOLINT(clang-diagnostic-switch-enum)
    {
        case ParticleType::Sand:
            color = {255, 255, 0, 255};
            break;
        case ParticleType::Water:
            color = {0, 0, 255, 255};
            break;
        default:
            color = {};
            break;
    }
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}

void Filed::SwapParticles(const int x1, const int y1, const int x2, const int y2) const
{
    const int i1 = Ind(x1,y1);
    const int i2 = Ind(x2,y2);
    const ParticleType temp = m_particles[i1];
    
    m_particles[i1] = m_particles[i2];
    m_particles[i2] = temp;
}

void Filed::SwapParticles(const int i1, const int i2) const
{
    const ParticleType temp = m_particles[i1];
    
    m_particles[i1] = m_particles[i2];
    m_particles[i2] = temp;
}

bool Filed::CanSwap(int i1, int i2) const
{
    if (m_particles[i2] == ParticleType::None) return true;
    //if (m_particles[i1] == ParticleType::None) return true;
    
    if (m_particles[i1] == ParticleType::Sand && m_particles[i2] == ParticleType::Water) return true;
    //if (m_particles[i2] == ParticleType::Sand && m_particles[i1] == ParticleType::Water) return true;

    return false;
}

int Filed::Ind(const int x, const int y) const
{
    return x + y * m_width;
}

