#include "Field.h"

#include <iostream>

#include "Config.h"
#include "Particle.h"
#include "SDL_egl.h"
#include "TheGame.h"


using namespace ParticleDefinitions;

Field::Field(): m_RawTexturePtr(nullptr)
{
    m_width = TheGame::WIDTH;
    m_height = TheGame::HEIGHT;
    m_size = m_height * m_width;
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

Field::~Field()
{
    delete[] m_particlesGrid;
    SDL_DestroyTexture(m_texture);
#if ACTIVE_PARTICLES_DEBUG_VIEW
    SDL_DestroyTexture(d_debugActiveTexture);
#endif
}

void Field::Render()
{
    if (m_isTextureLocked)
    {
        SDL_UnlockTexture(m_texture);
        m_isTextureLocked = false;
    }

    SDL_RenderCopy(m_renderer, m_texture , nullptr, nullptr);
    
#if ACTIVE_PARTICLES_DEBUG_VIEW
    SDL_LockTexture(d_debugActiveTexture, nullptr, reinterpret_cast<void**>(&d_debugRawTexturePointer), &m_pitch);
    memset(d_debugRawTexturePointer, 0, m_size * sizeof(Uint32));
    for (auto& p : m_particles)
    {
         const int ind = Ind(p.position);
         if (p.isActive)
         {
             d_debugRawTexturePointer[ind] = 0 << 24 | 255 << 16 | 0 << 8 | 255;
         }
         else
         {
             d_debugRawTexturePointer[ind] = 255 << 24 | 0 << 16 | 0 << 8 | 255;
         }
    }
    SDL_UnlockTexture(d_debugActiveTexture);
     
    SDL_RenderCopy(m_renderer, d_debugActiveTexture, nullptr, nullptr);
#endif
        
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Field::Update()
{
    for (auto& particle : m_particles)
    {
        // if (!particle.isActive) continue;
        UpdateParticle(particle);
    }
}

void Field::UpdateParticle(Particle& particle)
{
    const Uint8 actions = GetActionsByType(particle.type);
    if (!HasAction(actions, ParticleAction::IMMOVABLE))
    {
        if (HasAction(actions, ParticleAction::MOVE_HORIZONTAL))
        {
            if (particle.isGrounded)
                AddHorizontalVelocity(particle);
        }
        if (HasAction(actions, ParticleAction::MOVE_VERTICAL))
        {
            if (!particle.isGrounded)
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

void Field::AddVerticalVelocity(Particle& particle)
{
    const float velocityAdded = GetMassByType(particle.type)
                        *  Config::G_GRAVITY * TheGame::Instance().GetDeltaTime();
    particle.velocity.y += velocityAdded;
}

void Field::AddHorizontalVelocity(Particle& particle) const
{
    float velocityAdded = 0;
    if (particle.velocity.x > 0 &&
        m_particlesGrid[Ind(particle.position + IVec2(1, 0))] == nullptr)
    {
        velocityAdded = GetMassByType(particle.type)
                        *  Config::G_SIDE_SPEED * TheGame::Instance().GetDeltaTime() ;
    }
    else if (particle.velocity.x < m_width - 1 &&
             m_particlesGrid[Ind(particle.position + IVec2(-1, 0))] == nullptr)
    {
         velocityAdded = -(GetMassByType(particle.type)
                         *  Config::G_SIDE_SPEED * TheGame::Instance().GetDeltaTime() );
    }
    particle.velocity.x += velocityAdded;
}

void Field::MoveParticle(Particle& particle)
{
    IVec2 beginningPos = particle.position;
    const int vX = static_cast<int>(particle.velocity.x);
    const int vY = static_cast<int>(particle.velocity.y);

    IVec2 start = particle.position;
    const IVec2 end = start + IVec2(vX, vY);
    int dx = std::abs(end.x - start.x);
    int dy = -std::abs(end.y - start.y);

    IVec2 s;
    if (start.x < end.x)
        s.x = 1;
    else
        s.x = -1;

    if (start.y < end.y)
        s.y = 1;
    else
        s.y = -1;

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
    if (particle.velocity == Vec2(0, 0))
        particle.isActive = false;
    else
        particle.velocity = particle.velocity * 0.99;
}

bool Field::CanSwapParticles(const Particle& particle, const IVec2& direction) const
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
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else if (IsLiquid(actions))
    {
        if (IsGas(otherActions))
        {
            return true;
        }
        else if (IsLiquid(otherActions))
        {
            if (GetMassByType(particle.type) > GetMassByType(otherParticle->type))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else // solid
        {
            return false;
        }
    }
    else if (IsSolid(actions))
    {
        if (particle.type == ParticleType::Sand && otherParticle->type == ParticleType::Water)
        {
            return true;
        }
        if (IsSolid(otherActions))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        throw std::exception("Missing particle type in actions");
    }
}

