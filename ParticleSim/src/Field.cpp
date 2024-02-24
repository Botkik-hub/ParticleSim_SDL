#include "Field.h"

#include <iostream>

#include "Particle.h"
#include "TheGame.h"

Field::Field(): m_RawTexturePtr(nullptr)
{
    m_width = TheGame::WIDTH;
    m_height = TheGame::HEIGHT;
    m_size = m_height * m_width;
    m_renderer = TheGame::Instance().GetRenderer();

    Uint32* textureColors = new Uint32[m_size];

    m_frameToUpdateFlag = true;
    m_particles = new Particle[m_size];
    m_pitch = sizeof(Uint32) * m_width;
    m_isTextureLocked = false;

    for (int i = 0; i < m_size; ++i)
    {
        m_particles[i].type = ParticleType::None;
        textureColors[i] = ColToUint({0, 0, 0, 255});
    }

    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
                                  SDL_TEXTUREACCESS_STREAMING, m_width, m_height);

    SDL_UpdateTexture(m_texture, nullptr, textureColors, m_pitch);
    delete[] textureColors;
}


Field::~Field()
{
    delete[] m_particles;
    SDL_DestroyTexture(m_texture);
}

void Field::UpdateParticle(int x, int y)
{
    const Uint8 actions = m_particles[Ind(x, y)].actions;
    
    if (actions == 0) return;

    if ((actions & ParticleAction::MoveDown) != 0)
        UpdateFall(x, y);

    if ((actions & ParticleAction::MoveUp) != 0)
        UpdateRaise(x, y);
    
    if ((actions & ParticleAction::MoveSides) != 0)
        UpdateSides(x, y);
}

void Field::Update(float dt)
{
    m_frameToUpdateFlag = !m_frameToUpdateFlag;

    for (int i = m_size- 1; i >= 0; --i)
    {
        if (m_particles[i].type == ParticleType::None) continue;
        if (m_particles[i].frameToUpdateFlag == m_frameToUpdateFlag) continue;
        m_particles[i].frameToUpdateFlag = m_frameToUpdateFlag;
        m_particles[i].isActive = true;
        int x, y;
        Cord(i, x, y);
        UpdateParticle(x, y);
    }
}


void Field::Render()
{
    if (m_isTextureLocked)
    {
        SDL_UnlockTexture(m_texture);
        m_isTextureLocked = false;
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

void Field::Cord(int i, int& x, int& y) const
{
    x = i % m_width;
    y = i / m_width;
}

void Field::UpdateTexture(int ind, Uint32 color)
{
    if (!m_isTextureLocked)
    {
        SDL_LockTexture(m_texture, nullptr, reinterpret_cast<void**>(&m_RawTexturePtr), &m_pitch);
        m_isTextureLocked = true;
    }
    m_RawTexturePtr[ind] = color;
}

void Field::SpawnParticle(int x, int y, ParticleType type)
{
    const int ind = Ind(x,y);
    m_particles[ind].type= type;
    m_particles[ind].actions = ParticleDefinitions::GetActionsByType(type);
    UpdateTexture(ind, ParticleDefinitions::GetColorByType(type));
}


void Field::UpdateFall(int x, int y)
{
    if (y >= m_height - 1) return;

    const int ind = Ind(x, y);
    const int indUnder = Ind(x, y + 1);
    const int indUnderLeft = Ind(x - 1, y + 1);
    const int indUnderRight = Ind(x + 1, y + 1);

    const bool hasLeft = x < m_width - 1;
    const bool hasRight = x > 0;
    
    if (!m_particles[ind].isActive) return;

    // Drop down
    if (m_particles[indUnder].type == ParticleType::None)
    {
        m_particles[ind].isActive = false;
        SwapParticles(ind, indUnder);
        return;
    }
    else if (ParticleDefinitions::GetDensityByType(m_particles[ind].type) > ParticleDefinitions::GetDensityByType(m_particles[indUnder].type) &&
                         ParticleDefinitions::CanBeMoved(m_particles->actions))
    {
        // check if can move left/right
        const int indLeft = Ind(x - 1, y);
        const int indRight = Ind(x+1, y);
        if (hasLeft && m_particles[indLeft].type == ParticleType::None)
        {
            m_particles[ind].isActive = true;
            SwapParticles(indLeft, indUnder);
            SwapParticles(ind, indUnder);
            return;
        }
        if (hasRight && m_particles[indRight].type == ParticleType::None)
        {
             m_particles[ind].isActive = true;
             SwapParticles(indRight, indUnder);
             SwapParticles(ind, indUnder);
             return;   
        }
        m_particles[ind].isActive = false;
        SwapParticles(ind, indUnder);
        return; 
    }
    
    if (hasRight &&
        (m_particles[indUnderLeft].type == ParticleType::None ||
        (ParticleDefinitions::GetDensityByType(m_particles[ind].type) > ParticleDefinitions::GetDensityByType(m_particles[indUnderLeft].type) &&
            ParticleDefinitions::CanBeMoved(m_particles[indUnderLeft].actions))))
    {
        m_particles[ind].isActive = false;
        SwapParticles(ind, indUnderLeft);
        return;
    }
    
    if (hasLeft && (m_particles[indUnderRight].type == ParticleType::None ||
    (ParticleDefinitions::GetDensityByType(m_particles[ind].type) > ParticleDefinitions::GetDensityByType(m_particles[indUnderRight].type) &&
        ParticleDefinitions::CanBeMoved(m_particles[indUnderRight].actions))))
    {
        m_particles[ind].isActive = false;
        SwapParticles(ind, indUnderRight);
        return;
    }
}

void Field::UpdateRaise(int x, int y)
{
    if (y <= 0) return;

    const int ind = Ind(x, y);
    const int indAbove = Ind(x, y - 1);
    
    if (!m_particles[ind].isActive) return;
    
    if (m_particles[indAbove].type == ParticleType::None ||
        (ParticleDefinitions::GetDensityByType(m_particles[ind].type) < ParticleDefinitions::GetDensityByType(m_particles[indAbove].type) &&
        ParticleDefinitions::CanBeMoved(m_particles[indAbove].actions)))
    {
        m_particles[ind].isActive = false;
        SwapParticles(ind, indAbove);
        return;
    }
    const int indAboveLeft = Ind(x - 1, y - 1);
    if (x > 0 &&
        (m_particles[indAboveLeft].type == ParticleType::None ||
        (ParticleDefinitions::GetDensityByType(m_particles[ind].type) < ParticleDefinitions::GetDensityByType(m_particles[indAboveLeft].type) &&
            ParticleDefinitions::CanBeMoved(m_particles[indAboveLeft].actions))))
    {
        m_particles[ind].isActive = false;
        SwapParticles(ind, indAboveLeft);
        return;
    }
    const int indAboveRight = Ind(x + 1, y - 1);
    if (x < m_width - 1 && (m_particles[indAboveRight].type == ParticleType::None ||
        (ParticleDefinitions::GetDensityByType(m_particles[ind].type) < ParticleDefinitions::GetDensityByType(m_particles[indAboveRight].type) &&
            ParticleDefinitions::CanBeMoved(m_particles[indAboveRight].actions))))
    {
        m_particles[ind].isActive = false;
        SwapParticles(ind, indAboveRight);
        return;
    }
}

void Field::UpdateSides(int x, int y)
{
    const int ind = Ind(x, y);
    const int indLeft = Ind(x - 1, y);
    
    if (!m_particles[ind].isActive) return;
    
    if (x > 0 && m_particles[indLeft].type == ParticleType::None)
    {
        m_particles[ind].isActive = false;
        SwapParticles(ind, indLeft);
        return;
    }
    const int indRight = Ind(x + 1, y);
    if (x < m_width - 1 && m_particles[indRight].type == ParticleType::None)
    {
        m_particles[ind].isActive = false;
        SwapParticles(ind, indRight);
        return;
    }
}

void Field::SwapParticles(const int ind, const int indOther)
{
    const Particle temp = m_particles[ind];
    m_particles[ind] = m_particles[indOther];
    m_particles[indOther] = temp;
    UpdateTexture(ind, ParticleDefinitions::GetColorByType(m_particles[ind].type));
    UpdateTexture(indOther, ParticleDefinitions::GetColorByType(m_particles[indOther].type));
}