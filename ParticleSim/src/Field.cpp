#include "Field.h"

#include <iostream>

#include "Config.h"
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
        m_particles[i].velocity ={0, 0};
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

void Field::UpdateParticle(int x, int y, int index)
{
    UpdateVelocity(index, {x, y});
    UpdateMovement(index, {x, y});
    // const Uint8 actions = m_particles[Ind(x, y)].actions;
    //
    // if (actions == 0) return;
    //
    // if ((actions & ParticleAction::MOVE_DOWN) != 0)
    //     UpdateFall(x, y, index);
    //
    // if ((actions & ParticleAction::MOVE_UP) != 0)
    //     UpdateRaise(x, y, index);
    //
    // if ((actions & ParticleAction::MOVE_SIDES) != 0)
    //     UpdateSides(x, y, index);
}

void Field::UpdateVelocity(int index, IVec2 cord)
{
    const Uint8 actions = m_particles[index].actions;

    // if no block underneath or up and action is up or down
    // if not that, check sides if action

    if (ParticleDefinitions::HasAction(actions, ParticleAction::MOVE_VERTICAL))
    {
        AddVerticalVelocity(index, cord);
    }

    // where to do check if should move to sides
    if (ParticleDefinitions::HasAction(actions, ParticleAction::MOVE_HORIZONTAL))
    {
        if (IsSomethingUnder(cord))
            AddHorizontalVelocity(index, cord);
    }
}

void Field::AddVerticalVelocity(const int index, IVec2 cord)
{
    const float velocityAdded = ParticleDefinitions::GetMassByType(m_particles[index].type)
                        *  Config::GRAVITY * TheGame::Instance().GetDeltaTime();
    m_particles[index].velocity.y += velocityAdded;
}

void Field::AddHorizontalVelocity(int index, IVec2 cord)
{
    const float velocityAdded = ParticleDefinitions::GetMassByType(m_particles[index].type)
                            *  Config::SIDE_SPEED * TheGame::Instance().GetDeltaTime() / 2;
    m_particles[index].velocity.x += velocityAdded;
}

void Field::UpdateMovement(int index, IVec2 cord)
{
    int dX = m_particles[index].velocity.x;
    int dY = m_particles[index].velocity.y;

    // lets move for now all y then all x
    // TODO move diagonally

    while (dY != 0)
    {
        if (dY > 0)
        {
            // move down 
            if (cord.y >= m_height)
            {
                m_particles[index].velocity.x = 0;
                break;
            }
            UpdateFall(cord.x, cord.y, index);
            dY -= 1;
        }
        else
        {
            // move up
             if (cord.y < 0)
             {
                 m_particles[index].velocity.x = 0;
                 break;
             }
             UpdateRaise(cord.x, cord.y, index);
             dY += 1;   
        }
    }
    while (dX != 0)
    {
         UpdateSides(cord.x, cord.y, index);
        if (dX > 0)
            dX -= 1;
        else
            dX += 1;
     }
}

void Field::Update()
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
        UpdateParticle(x, y, i);
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

int Field::Ind(const IVec2& cord) const
{
    return cord.x + cord.y * m_width;
}

void Field::Cord(int i, int& x, int& y) const
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


void Field::UpdateFall(int x, int y, int index)
{
    if (y >= m_height - 1) return;

    const int indUnder = Ind(x, y + 1);
    const int indUnderLeft = Ind(x - 1, y + 1);
    const int indUnderRight = Ind(x + 1, y + 1);

    const bool hasLeft = x < m_width - 1;
    const bool hasRight = x > 0;
    
    if (!m_particles[index].isActive) return;

    // Drop down
    if (m_particles[indUnder].type == ParticleType::None)
    {
        m_particles[index].isActive = false;
        SwapParticles(index, indUnder);
        return;
    }
    else if (ParticleDefinitions::GetMassByType(m_particles[index].type) > ParticleDefinitions::GetMassByType(m_particles[indUnder].type) &&
                         ParticleDefinitions::CanBeMoved(m_particles->actions))
    {
        // check if can move left/right
        const int indLeft = Ind(x - 1, y);
        const int indRight = Ind(x+1, y);
        if (hasLeft && m_particles[indLeft].type == ParticleType::None)
        {
            m_particles[index].isActive = true;
            SwapParticles(indLeft, indUnder);
            SwapParticles(index, indUnder);
            return;
        }
        if (hasRight && m_particles[indRight].type == ParticleType::None)
        {
             m_particles[index].isActive = true;
             SwapParticles(indRight, indUnder);
             SwapParticles(index, indUnder);
             return;   
        }
        m_particles[index].isActive = false;
        SwapParticles(index, indUnder);
        return; 
    }
    
    if (hasRight &&
        (m_particles[indUnderLeft].type == ParticleType::None ||
        (ParticleDefinitions::GetMassByType(m_particles[index].type) > ParticleDefinitions::GetMassByType(m_particles[indUnderLeft].type) &&
            ParticleDefinitions::CanBeMoved(m_particles[indUnderLeft].actions))))
    {
        m_particles[index].isActive = false;
        SwapParticles(index, indUnderLeft);
        return;
    }
    
    if (hasLeft && (m_particles[indUnderRight].type == ParticleType::None ||
    (ParticleDefinitions::GetMassByType(m_particles[index].type) > ParticleDefinitions::GetMassByType(m_particles[indUnderRight].type) &&
        ParticleDefinitions::CanBeMoved(m_particles[indUnderRight].actions))))
    {
        m_particles[index].isActive = false;
        SwapParticles(index, indUnderRight);
        return;
    }
}

void Field::UpdateRaise(int x, int y, int index)
{
    if (y <= 0) return;

    const int indAbove = Ind(x, y - 1);
    
    if (!m_particles[index].isActive) return;
    
    if (m_particles[indAbove].type == ParticleType::None ||
        (ParticleDefinitions::GetMassByType(m_particles[index].type) < ParticleDefinitions::GetMassByType(m_particles[indAbove].type) &&
        ParticleDefinitions::CanBeMoved(m_particles[indAbove].actions)))
    {
        m_particles[index].isActive = false;
        SwapParticles(index, indAbove);
        return;
    }
    const int indAboveLeft = Ind(x - 1, y - 1);
    if (x > 0 &&
        (m_particles[indAboveLeft].type == ParticleType::None ||
        (ParticleDefinitions::GetMassByType(m_particles[index].type) < ParticleDefinitions::GetMassByType(m_particles[indAboveLeft].type) &&
            ParticleDefinitions::CanBeMoved(m_particles[indAboveLeft].actions))))
    {
        m_particles[index].isActive = false;
        SwapParticles(index, indAboveLeft);
        return;
    }
    const int indAboveRight = Ind(x + 1, y - 1);
    if (x < m_width - 1 && (m_particles[indAboveRight].type == ParticleType::None ||
        (ParticleDefinitions::GetMassByType(m_particles[index].type) < ParticleDefinitions::GetMassByType(m_particles[indAboveRight].type) &&
            ParticleDefinitions::CanBeMoved(m_particles[indAboveRight].actions))))
    {
        m_particles[index].isActive = false;
        SwapParticles(index, indAboveRight);
        return;
    }
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

void Field::SwapParticles(const int ind, const int indOther)
{
    const Particle temp = m_particles[ind];
    m_particles[ind] = m_particles[indOther];
    m_particles[indOther] = temp;
    UpdateTexture(ind, ParticleDefinitions::GetColorByType(m_particles[ind].type));
    UpdateTexture(indOther, ParticleDefinitions::GetColorByType(m_particles[indOther].type));
}

bool Field::IsSomethingUnder(const IVec2 cord) const
{
    if (cord.y == m_height - 1)
        return true;

    return m_particles[Ind(cord.x, cord.y)].type != ParticleType::None;
   
}
