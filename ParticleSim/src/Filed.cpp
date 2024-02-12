#include "Filed.h"

#include "TheGame.h"

Filed::Filed()
{
    m_particles = new ParticleType*[TheGame::WIDTH];
    for (int i = 0; i < TheGame::WIDTH; ++i)
    {
        m_particles[i] = new ParticleType[TheGame::HEIGHT];
        for (int j = 0; j < TheGame::HEIGHT; ++j)
        {
            m_particles[i][j] = ParticleType::None;
        }
    }
    m_renderer = TheGame::Instance().GetRenderer();

    for (int x = 100; x < 200; ++x)
    {
        for (int y = 50; y < 100; ++y)
        {
            m_particles[x][y] = ParticleType::Sand;
        }
    }

    for (int x = 200; x < TheGame::WIDTH; ++x)
    {
        for (int y = 50; y < 100; ++y)
        {
            m_particles[x][y] = ParticleType::Water;
        }
    }
}

Filed::~Filed()
{
    for (int i = 0; i < TheGame::WIDTH; ++i)
    {
        delete m_particles[i];
    }
    delete m_particles;
}

void Filed::Update(float dt)
{
    for (int x = TheGame::WIDTH - 1; x >= 0; --x)
    {
        for (int y = TheGame::HEIGHT - 2; y >= 0; --y)
        {
            if (m_particles[x][y] != ParticleType::None)
            {
                if (m_particles[x][y + 1] == ParticleType::None)
                {
                    m_particles[x][y + 1] = m_particles[x][y];
                    m_particles[x][y] = ParticleType::None;
                }
                else if (x - 1 > 0 && m_particles[x - 1][y + 1] == ParticleType::None)
                {
                    m_particles[x-1][y+1] = m_particles[x][y];
                    m_particles[x][y] = ParticleType::None;
                }
                else if (x + 1 < TheGame::WIDTH && m_particles[x + 1][y + 1] == ParticleType::None)
                {
                    m_particles[x+1][y+1] = m_particles[x][y];
                    m_particles[x][y] = ParticleType::None;
                }
            }
        }
    }
}

void Filed::Render() const
{
    for (int x = 0; x < TheGame::WIDTH; ++x)
    {
        for (int y = 0; y < TheGame::HEIGHT; ++y)
        {
            if (m_particles[x][y] == ParticleType::None) continue;
            SetColor(m_particles[x][y]);
            SDL_RenderDrawPoint(m_renderer, x, y);
        }
    }
}

void Filed::SetColor(const ParticleType type) const
{
    SDL_Color color;
    switch (type)
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

void Filed::SwapParticles(int x1, int y1, int x2, int y2)
{
    
}
