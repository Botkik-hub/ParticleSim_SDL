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
}

Field::~Field()
{
    delete[] m_particlesGrid;
    SDL_DestroyTexture(m_texture);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Field::Update()
{
    for (auto& particle : m_particles)
    {
        if (!particle.isActive) continue;
        UpdateParticle(particle);
    }
}

void Field::UpdateParticle(Particle& particle)
{
    Uint8 actions = GetActionsByType(particle.type);
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
}

void Field::AddVerticalVelocity(Particle& particle)
{
    const float velocityAdded = ParticleDefinitions::GetMassByType(particle.type)
                        *  Config::GRAVITY * TheGame::Instance().GetDeltaTime();
    particle.velocity.y += velocityAdded;
}

void Field::AddHorizontalVelocity(Particle& particle)
{
    const float velocityAdded = ParticleDefinitions::GetMassByType(particle.type)
                            *  Config::SIDE_SPEED * TheGame::Instance().GetDeltaTime() / 2;
    particle.velocity.x += velocityAdded;
}

void Field::MoveParticle(Particle& particle)
{
    int vX = (int)particle.velocity.x;
    int vY = (int)particle.velocity.y;

    IVec2 start = particle.position;
    IVec2 end = start + IVec2(vX, vY);
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
            else
            {
                if (change.x != 0)
                {
                    start.x = end.x;
                    dx = 0;
                    error = dy;
                }
                if (change.y != 0)
                {
                    start.y = end.y;
                    dy = 0;
                    error = dx;
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Field::UpdateFall(int x, int y, int index)
{
//     if (y >= m_height - 1) return;
//
//     const int indUnder = Ind(x, y + 1);
//     const int indUnderLeft = Ind(x - 1, y + 1);
//     const int indUnderRight = Ind(x + 1, y + 1);
//
//     const bool hasLeft = x < m_width - 1;
//     const bool hasRight = x > 0;
//     
//     if (!m_particles[index].isActive) return;
//
//     // Drop down
//     if (m_particles[indUnder].type == ParticleType::None)
//     {
//         m_particles[index].isActive = false;
//         SwapParticles(index, indUnder);
//         return;
//     }
//     else if (ParticleDefinitions::GetMassByType(m_particles[index].type) > ParticleDefinitions::GetMassByType(m_particles[indUnder].type) &&
//                          ParticleDefinitions::CanBeMoved(m_particles->actions))
//     {
//         // check if can move left/right
//         const int indLeft = Ind(x - 1, y);
//         const int indRight = Ind(x+1, y);
//         if (hasLeft && m_particles[indLeft].type == ParticleType::None)
//         {
//             m_particles[index].isActive = true;
//             SwapParticles(indLeft, indUnder);
//             SwapParticles(index, indUnder);
//             return;
//         }
//         if (hasRight && m_particles[indRight].type == ParticleType::None)
//         {
//              m_particles[index].isActive = true;
//              SwapParticles(indRight, indUnder);
//              SwapParticles(index, indUnder);
//              return;   
//         }
//         m_particles[index].isActive = false;
//         SwapParticles(index, indUnder);
//         return; 
//     }
//     
//     if (hasRight &&
//         (m_particles[indUnderLeft].type == ParticleType::None ||
//         (ParticleDefinitions::GetMassByType(m_particles[index].type) > ParticleDefinitions::GetMassByType(m_particles[indUnderLeft].type) &&
//             ParticleDefinitions::CanBeMoved(m_particles[indUnderLeft].actions))))
//     {
//         m_particles[index].isActive = false;
//         SwapParticles(index, indUnderLeft);
//         return;
//     }
//     
//     if (hasLeft && (m_particles[indUnderRight].type == ParticleType::None ||
//     (ParticleDefinitions::GetMassByType(m_particles[index].type) > ParticleDefinitions::GetMassByType(m_particles[indUnderRight].type) &&
//         ParticleDefinitions::CanBeMoved(m_particles[indUnderRight].actions))))
//     {
//         m_particles[index].isActive = false;
//         SwapParticles(index, indUnderRight);
//         return;
//     }
}

void Field::UpdateRaise(int x, int y, int index)
{
    // if (y <= 0) return;
    //
    // const int indAbove = Ind(x, y - 1);
    //
    // if (!m_particles[index].isActive) return;
    //
    // if (m_particles[indAbove].type == ParticleType::None ||
    //     (ParticleDefinitions::GetMassByType(m_particles[index].type) < ParticleDefinitions::GetMassByType(m_particles[indAbove].type) &&
    //     ParticleDefinitions::CanBeMoved(m_particles[indAbove].actions)))
    // {
    //     m_particles[index].isActive = false;
    //     SwapParticles(index, indAbove);
    //     return;
    // }
    // const int indAboveLeft = Ind(x - 1, y - 1);
    // if (x > 0 &&
    //     (m_particles[indAboveLeft].type == ParticleType::None ||
    //     (ParticleDefinitions::GetMassByType(m_particles[index].type) < ParticleDefinitions::GetMassByType(m_particles[indAboveLeft].type) &&
    //         ParticleDefinitions::CanBeMoved(m_particles[indAboveLeft].actions))))
    // {
    //     m_particles[index].isActive = false;
    //     SwapParticles(index, indAboveLeft);
    //     return;
    // }
    // const int indAboveRight = Ind(x + 1, y - 1);
    // if (x < m_width - 1 && (m_particles[indAboveRight].type == ParticleType::None ||
    //     (ParticleDefinitions::GetMassByType(m_particles[index].type) < ParticleDefinitions::GetMassByType(m_particles[indAboveRight].type) &&
    //         ParticleDefinitions::CanBeMoved(m_particles[indAboveRight].actions))))
    // {
    //     m_particles[index].isActive = false;
    //     SwapParticles(index, indAboveRight);
    //     return;
    // }
}

void Field::UpdateSides(int x, int y, int index)
{
    const int indLeft = Ind(x - 1, y);
    
    if (!m_particles[index].isActive) return;
    
    if (x > 0 && m_particles[indLeft].type == ParticleType::None)
    {
        m_particles[index].isActive = false;
        SwapParticles(index, indLeft);
        return;
    }
    const int indRight = Ind(x + 1, y);
    if (x < m_width - 1 && m_particles[indRight].type == ParticleType::None)
    {
        m_particles[index].isActive = false;
        SwapParticles(index, indRight);
        return;
    }
}

bool Field::IsSomethingUnder(const IVec2 cord) const
{
    if (cord.y == m_height - 1)
        return true;
    return m_particles[Ind(cord.x, cord.y)].type != ParticleType::None;
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
    
    if (IsGas(actions))
    {
        if (IsGas(otherActions))
        {
            if (GetMassByType(particle.type) >= GetMassByType(otherParticle->type))
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
            if (GetMassByType(particle.type) >= GetMassByType(otherParticle->type))
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

