#include "FieldChunk.h"

#include <iostream>

#include "Config.h"
#include "Particle.h"
#include "SDL_egl.h"
#include "TheGame.h"


using namespace ParticleDefinitions;

FieldChunk::FieldChunk(const IVec2 position, const IVec2 size, const IVec2 dstSize): m_RawTexturePtr(nullptr)
{
    m_position = position;
    m_width = size.x;
    m_height = size.y;
    m_size = m_height * m_width;
    m_dst.x = position.x;
    m_dst.y = position.y;
    m_dst.w = dstSize.x;
    m_dst.h = dstSize.y;
    m_renderer = TheGame::Instance().GetRenderer();

    Uint32* textureColors = new Uint32[m_size];
    m_pitch = sizeof(Uint32) * m_width;
    m_isTextureLocked = false;
    
    m_particlesGrid = new Particle*[m_size];
    m_particles.reserve(m_size);
    for (int i = 0; i < m_size; ++i)
    {
        m_particlesGrid[i] = nullptr;
        textureColors[i] = ColToUint({0, 0, 0, 255});
    }

    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
                                  SDL_TEXTUREACCESS_STREAMING, m_width, m_height);

    SDL_UpdateTexture(m_texture, nullptr, textureColors, m_pitch);
    delete[] textureColors;

#if ACTIVE_PARTICLES_DEBUG_VIEW
    d_debugActiveTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
    SDL_SetTextureAlphaMod(d_debugActiveTexture, 255);
    SDL_SetTextureBlendMode(d_debugActiveTexture, SDL_BLENDMODE_BLEND);
#endif
}

FieldChunk::~FieldChunk()
{
    delete[] m_particlesGrid;
    SDL_DestroyTexture(m_texture);
#if ACTIVE_PARTICLES_DEBUG_VIEW
    SDL_DestroyTexture(d_debugActiveTexture);
#endif
}

void FieldChunk::Render()
{
    if (m_isTextureLocked)
    {
        SDL_UnlockTexture(m_texture);
        m_isTextureLocked = false;
    }
    
    SDL_RenderCopy(m_renderer, m_texture , nullptr, &m_dst);
    
#if ACTIVE_PARTICLES_DEBUG_VIEW
    SDL_LockTexture(d_debugActiveTexture, nullptr, reinterpret_cast<void**>(&d_debugRawTexturePointer), &m_pitch);
    memset(d_debugRawTexturePointer, 0, m_size * sizeof(Uint32));
    for (auto& p : m_particles)
    {
        const int ind = Ind(p.position);
        if (p.isActive) //velocity.x == 0)
        {
            d_debugRawTexturePointer[ind] = 255  << 24 | 255 << 16 | 255  << 8 | 255;
        }
        // else if (p.velocity.x > 0)
        // {
        //     d_debugRawTexturePointer[ind] = 0 << 24 | 255 << 16 | 0 << 8 | 255;
        // }
        else
        {
            d_debugRawTexturePointer[ind] = 255 << 24 | 0 << 16 | 0 << 8 | 255;
        }
    }
    SDL_UnlockTexture(d_debugActiveTexture);
     
    SDL_RenderCopy(m_renderer, d_debugActiveTexture, nullptr, nullptr);
#endif
}

void FieldChunk::Update()
{
    for (auto& particle : m_particles)
    {
        if (!particle.isActive) continue;
        UpdateParticle(particle);
    }
}

void FieldChunk::UpdateParticle(Particle& particle)
{
    const Uint8 actions = GetActionsByType(particle.type);
    if (!HasAction(actions, ParticleAction::IMMOVABLE))
    {
        bool canGoDown = CanGoDown(particle);
        if (HasAction(actions, ParticleAction::MOVE_HORIZONTAL))
        {
            if (!canGoDown)
                AddHorizontalVelocity(particle);
        }
        if (HasAction(actions, ParticleAction::MOVE_VERTICAL))
        {
            if (canGoDown)
                AddVerticalVelocity(particle);
        }
        
        MoveParticle(particle);
    }
    else
    {
        // todo  remove this and add other updates 
        particle.isActive = false;
    }
}

void FieldChunk::AddVerticalVelocity(Particle& particle)
{
    const float velocityAdded = GetMassByType(particle.type)
                        *  Config::G_GRAVITY * TheGame::Instance().GetDeltaTime();
    particle.velocity.y += velocityAdded;
}

void FieldChunk::AddHorizontalVelocity(Particle& particle) const
{
    float velocityAdded = 0;
    if (particle.position == IVec2(107, 10))
    {
        bool test = false;
    }
    if (particle.velocity.x >= 0)
    {
        if (CanGoRight(particle))
            velocityAdded = std::abs(GetMassByType(particle.type))
                        *  Config::G_SIDE_SPEED * TheGame::Instance().GetDeltaTime() ;
        else
            particle.velocity.x = 0;
    }
    if (particle.velocity.x <= 0)
    {
        if (CanGoLeft(particle))
             velocityAdded = -(std::abs(GetMassByType(particle.type))
                         *  Config::G_SIDE_SPEED * TheGame::Instance().GetDeltaTime() );
        else
            particle.velocity.x = 0;
    }
    particle.velocity.x += velocityAdded;
}

void FieldChunk::MoveParticle(Particle& particle)
{
    const int vX = static_cast<int>(particle.velocity.x);
    const int vY = static_cast<int>(particle.velocity.y);

    if (vX == 0 && vY == 0) return;
    
    IVec2 start = particle.position;
    const IVec2 end = start + IVec2(vX, vY);
    
    int dx = std::abs(end.x - start.x);
    int dy = -std::abs(end.y - start.y);

    IVec2 s;
    s.x = start.x < end.x ? 1 : -1;
    s.y = start.y < end.y ? 1 : -1;

    int error = dx + dy;
    IVec2 change = {0, 0};
    
    while (true)
    {

        if (!(change == IVec2(0, 0)))
        {
            // stuff here
            if (CanSwapParticles(particle, change))
            {
                const int ind = Ind(particle.position);
                const int indOther = Ind(particle.position + change);
                SwapParticles(ind, indOther);
                start.x += change.x;
                start.y += change.y;
                change.x = 0;
                change.y = 0;
            }
            else if (change.y != 0 && dx >= 0  && CanSwapParticles(particle, change + IVec2(1, 0)))
            {
                const int ind = Ind(particle.position);
                const int indOther = Ind(particle.position + change + IVec2(1, 0));
                SwapParticles(ind, indOther);
                start.x += change.x + 1;
                start.y += change.y;
                change.x = 0;
                change.y = 0;
            }
            else if (change.y != 0 && dx <= 0 &&CanSwapParticles(particle, change + IVec2(-1, 0)))
            {
                const int ind = Ind(particle.position);
                const int indOther = Ind(particle.position + change + IVec2(-1, 0));
                SwapParticles(ind, indOther);
                start.x += change.x - 1;
                start.y += change.y;
                change.x = 0;
                change.y = 0;
            }
            else
            {
                if (change.x != 0)
                {
                    start.x = end.x;
                    dx = 0;
                    particle.velocity.x = 0;
                    error = dy;
                }
                if (change.y != 0)
                {
                    start.y = end.y;
                    dy = 0;
                    error = dx;
                    particle.velocity.y = 0;
                    particle.isGrounded = true;
                }
            }
        }
        
        if (start == end) break;

        const int error2 = error + error;
        if (error2 >= dy)
        {
            if (start.x == end.x) break;

            error = error + dy;
            change.x = s.x;
            // start.x = start.x + s.x;
        }
        if (error2 <= dx)
        {
            if (start.y == end.y) break;

            error = error + dx;
            change.y = s.y;
            // start.y = start.y + s.y;
        }
    }
        
    if (particle.velocity.SquareMagnitude() < 0.000001)
    {
        particle.isActive = false;
        // particle.velocity.x = 0;
        // particle.velocity.y = 0;
    }
    else
        particle.velocity = particle.velocity * Config::G_DAMPING;
}

bool FieldChunk::CanSwapParticles(const Particle& particle, const IVec2& direction) const
{
    // check in bounds
    const IVec2 pos2 = particle.position + direction;
    if (pos2.x < 0 || pos2.x >= m_width)
    {
        return false;
    }
    if (pos2.y < 0 || pos2.y >= m_height)
    {
        return false;
    }
    
    const Particle* otherParticle = m_particlesGrid[Ind(pos2)];
    
    if (otherParticle == nullptr)
    {
        return true;
    }

    return CanSwapParticles(particle.type, otherParticle->type);
    const Uint8 actions = GetActionsByType(particle.type);
    const Uint8 otherActions = GetActionsByType(otherParticle->type);

    if (HasAction(otherActions, ParticleAction::IMMOVABLE))
    {
        return false;
    }
    
    if (IsGas(actions))
    {
        if (IsGas(otherActions))
        {
            if (GetMassByType(particle.type) > GetMassByType(otherParticle->type))
                return true;
            return false;
        }
        return false;
    }
    if (IsLiquid(actions))
    {
        if (IsGas(otherActions))
        {
            return true;
        }
        if (IsLiquid(otherActions))
        {
            if (GetMassByType(particle.type) > GetMassByType(otherParticle->type))
                return true;
            return false;
        }
        return false;
    }
    else if (IsSolid(actions))
    {
        if (IsSolid(otherActions))
            return false;
        return true;
    }
    
    throw std::exception("Missing particle type in actions");
}

bool FieldChunk::CanGoDown(const Particle& particle) const
{
    // todo, not sure what to do if mass == 0
    
    const int offsetVertical = GetMassByType(particle.type) > 0 ? 1 : -1;

    
    // goes down
    if (offsetVertical > 0 && particle.position.y >= m_height -1 ) return false;
    if (offsetVertical < 0 && particle.position.y <= 0) return false;

    const Particle* under = m_particlesGrid[Ind(particle.position + IVec2(0, offsetVertical))];
    if (under == nullptr) return true;
    if (CanSwapParticles(particle.type, under->type)) return true;

    if (particle.position.x < m_width - 1)
    {
        const Particle* underRight = m_particlesGrid[Ind(particle.position + IVec2(1, offsetVertical))];
        if (underRight == nullptr) return true;
        if (CanSwapParticles(particle.type, underRight->type)) return true;
    }
    if (particle.position.x > 0)
    {
        const Particle* underLeft = m_particlesGrid[Ind(particle.position + IVec2(-1, offsetVertical))];
        if (underLeft == nullptr) return true;
        if (CanSwapParticles(particle.type, underLeft->type)) return true;
    }
    return false;
}

bool FieldChunk::CanGoRight(const Particle& particle) const
{
    if (particle.position.x >= m_width - 1) return false;

    if (m_particlesGrid[Ind(particle.position + IVec2(1, 0))] == nullptr)
    {
        return true;
    }
    return false;
}

bool FieldChunk::CanGoLeft(const Particle& particle) const
{
    if (particle.position.x <= 0) return false;

   
    if (m_particlesGrid[Ind(particle.position + IVec2(-1, 0))] == nullptr)
    {
        return true;
    }
    return false;
}

bool FieldChunk::CanSwapParticles(const ParticleType type, const ParticleType otherType)
{
    const Uint8 actions = GetActionsByType(type);
    const Uint8 otherActions = GetActionsByType(otherType);
    
    if (HasAction(otherActions, ParticleAction::IMMOVABLE))
    {
        return false;
    }
     
    if (IsGas(actions))
    {
        if (IsGas(otherActions))
        {
            if (GetMassByType(type) > GetMassByType(otherType))
            {
                return true;
            }
            return false;
        }
        return false;
    }
    if (IsLiquid(actions))
    {
        if (IsGas(otherActions))
        {
            return true;
        }
        if (IsLiquid(otherActions))
        {
            if (GetMassByType(type) > GetMassByType(otherType))
                return true;
            return false;
        }
        return false;
    }
    if (IsSolid(actions))
    {
        if (IsSolid(otherActions))
            return false;
        return true;
    }
    throw std::exception("Missing particle type in actions");
}

