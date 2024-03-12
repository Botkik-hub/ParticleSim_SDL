#include "Field.h"
#include "Particle.h"

int Field::Ind(const int x, const int y) const
{
    return x + y * m_width;
}

int Field::Ind(const IVec2& cord) const
{
    return cord.x + cord.y * m_width;
}

void Field::Cord(const int i, int& x, int& y) const
{
    x = i % m_width;
    y = i / m_width;
}

IVec2 Field::Cord(const int index) const
{
    return {
    index % m_width,
      index / m_width};  
}

Uint32 Field::ColToUint(const SDL_Color color)
{
    return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
}


void Field::UpdateTexture(const int ind, const Uint32 color)
{
    if (!m_isTextureLocked)
    {
        SDL_LockTexture(m_texture, nullptr, reinterpret_cast<void**>(&m_RawTexturePtr), &m_pitch);
        m_isTextureLocked = true;
    }
    m_RawTexturePtr[ind] = color;
}

void Field::SpawnParticle(int x, int y, const ParticleType type)
{
    m_particles.emplace_back();
    Particle& particle = m_particles.back();
    particle.position = {x, y};
    particle.type = type;
    particle.velocity = {0, 0};
    particle.isActive = true;

    const int index = Ind(x, y);
    m_particlesGrid[index] = &particle;
    UpdateTexture(index, ParticleDefinitions::GetColorByType(type));
}

void Field::SwapParticles(const int ind, const int indOther)
{
    Particle* one = m_particlesGrid[ind];
    Particle* two = m_particlesGrid[indOther];
    
    Particle* temp = m_particlesGrid[ind];
    m_particlesGrid[ind] = m_particlesGrid[indOther];
    m_particlesGrid[indOther] = temp;

    // swap positions here
    if (m_particlesGrid[ind] != nullptr)
    {
        m_particlesGrid[ind]->position = Cord(ind);
        UpdateTexture(ind, ParticleDefinitions::GetColorByType(m_particlesGrid[ind]->type));
    }
    else
    {
        UpdateTexture(ind, ParticleDefinitions::GetColorByType(ParticleType::None));
    }
    if (m_particlesGrid[indOther] != nullptr)
    {
        m_particlesGrid[indOther]->position = Cord(indOther);
        UpdateTexture(indOther, ParticleDefinitions::GetColorByType(m_particlesGrid[indOther]->type));
    }
    else
    {
        UpdateTexture(indOther, ParticleDefinitions::GetColorByType(ParticleType::None));
    }
}
