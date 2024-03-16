#include "FieldChunk.h"
#include "Particle.h"

int FieldChunk::Ind(const int x, const int y) const
{
    return x + y * m_width;
}

int FieldChunk::Ind(const IVec2& cord) const
{
    return cord.x + cord.y * m_width;
}

void FieldChunk::Cord(const int i, int& x, int& y) const
{
    x = i % m_width;
    y = i / m_width;
}

IVec2 FieldChunk::Cord(const int index) const
{
    return {
    index % m_width,
      index / m_width};  
}

Uint32 FieldChunk::ColToUint(const SDL_Color color)
{
    return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
}


void FieldChunk::UpdateTexture(const int ind, const Uint32 color)
{
    if (!m_isTextureLocked)
    {
        SDL_LockTexture(m_texture, nullptr, reinterpret_cast<void**>(&m_RawTexturePtr), &m_pitch);
        m_isTextureLocked = true;
    }
    m_RawTexturePtr[ind] = color;
}

void FieldChunk::SpawnParticle(int x, int y, const ParticleType type)
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

void FieldChunk::RemoveParticle(const Particle& particle)
{
    const auto it = std::find(m_particles.begin(), m_particles.end(), particle);
    if (it == m_particles.end()) return;

    const int index = std::distance(m_particles.begin(), it);
    
    m_particlesGrid[Ind(particle.position)] = nullptr;
    if (m_particles.size() != 1)
    {
        m_particlesGrid[Ind(m_particles.back().position)] = nullptr;
        m_particles[index] = m_particles.back();
        Particle& particleOldBack = m_particles[index];
        m_particlesGrid[Ind(particleOldBack.position)] = &particleOldBack;
    }
    m_particles.pop_back();
}

void FieldChunk::RemoveParticle(const int x, const int y)
{
    RemoveParticle(IVec2(x, y));
}

void FieldChunk::RemoveParticle(const IVec2 pos)
{
    const Particle* particle = m_particlesGrid[Ind(pos)];
    if (particle == nullptr) return;

    RemoveParticle(*particle);
}

Particle* FieldChunk::GetParticleAtPosition(const IVec2 position) const
{
    return m_particlesGrid[Ind(position)];
}

void FieldChunk::SwapParticles(const int ind, const int indOther)
{
    const Particle* one = m_particlesGrid[ind];
    const Particle* two = m_particlesGrid[indOther];

    if (one == nullptr && two == nullptr ) return;
    
    Particle* temp = m_particlesGrid[ind];
    m_particlesGrid[ind] = m_particlesGrid[indOther];
    m_particlesGrid[indOther] = temp;

    const IVec2 pos = Cord(ind);
    const IVec2 posOther = Cord(indOther);
    // swap positions here
    if (m_particlesGrid[ind] != nullptr)
    {
        m_particlesGrid[ind]->position = pos;
        UpdateTexture(ind, ParticleDefinitions::GetColorByType(m_particlesGrid[ind]->type));
        m_particlesGrid[ind]->isGrounded = false;
        m_particlesGrid[ind]->isActive = true;
    }
    else
    {
        UpdateTexture(ind, ParticleDefinitions::GetColorByType(ParticleType::None));
    }
    if (m_particlesGrid[indOther] != nullptr)
    {
        m_particlesGrid[indOther]->position = posOther;
        UpdateTexture(indOther, ParticleDefinitions::GetColorByType(m_particlesGrid[indOther]->type));
        m_particlesGrid[indOther]->isGrounded = false;
        m_particlesGrid[indOther]->isActive = true;
    }
    else
    {
        UpdateTexture(indOther, ParticleDefinitions::GetColorByType(ParticleType::None));
    }
    for (int dx = -1; dx <= 1; dx++)
    {
        const int x = pos.x + dx;
        if (x < 0 || x >= m_width) continue;
        
        for (int dy = -1; dy <= 1; dy++)
        {
            const int y = pos.y + dy;
            if (y < 0 || y >= m_height) continue;

            Particle* particle = m_particlesGrid[Ind(x, y)];
            if (particle != nullptr)
            {
                particle->isActive = true;
                particle->isGrounded = false;
            }
        }
    }
    for (int dx = -1; dx <= 1; dx++)
    {
        const int x = posOther.x + dx;
        if (x < 0 || x >= m_width) continue;
        
        for (int dy = -1; dy <= 1; dy++)
        {
            const int y = posOther.y + dy;
            if (y < 0 || y >= m_height) continue;

            Particle* particle = m_particlesGrid[Ind(x, y)];
            if (particle != nullptr)
            {
                particle->isActive = true;
                particle->isGrounded = false;
            }
        }
    }
}
