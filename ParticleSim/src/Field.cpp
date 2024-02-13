#include "Field.h"

#include <algorithm>

#include "TheGame.h"

Field::Field()
{
    m_width = TheGame::WIDTH;
    m_height = TheGame::HEIGHT;
    m_size = m_height * m_width;
    m_renderer = TheGame::Instance().GetRenderer();
    
    m_particles = new ParticleType[m_size];

    m_textureColors = new Uint32[m_size];
    m_needUpdateTexture = false;

    m_texture = SDL_CreateTexture(m_renderer,  SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, m_width, m_height);

    for (int i = 0; i < m_size; ++i)
    {
        m_particles[i] = ParticleType::None;
        m_textureColors[i] = ColToUint({0, 0, 0, 255});
    }
    
    for (int x = 170; x < 200; ++x)
    {
        for (int y = 50; y < 100; ++y)
        {
            m_particles[Ind(x,y)] = ParticleType::Sand;
            m_textureColors[Ind(x,y)] = ColToUint({255, 255, 0, 255});
        }
    }
    
    for (int x = 200; x < m_width; ++x)
    {
        for (int y = 50; y < 100; ++y)
        {
            m_particles[Ind(x,y)] = ParticleType::Water;
            m_textureColors[Ind(x,y)] = ColToUint({0, 0, 255, 255});
        }
    }
    SDL_UpdateTexture(m_texture, nullptr, m_textureColors, m_width * sizeof(Uint32));
    m_needUpdateTexture = false;
}

Field::~Field()
{
    delete[] m_particles;
    delete[] m_textureColors;
    SDL_DestroyTexture(m_texture);
}

void Field::ApplyGravity(int x, int y)
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
        SwapParticles(current,bottom);
    }
    else if (x - 1 > 0 && CanSwap(current, bottomLeft))
    {
        SwapParticles(current,bottomLeft);
    }
    else if (x + 1 < m_width && CanSwap(current, bottomRight))
    {
        SwapParticles(current, bottomRight);
    }
    else
    {
    }
}

void Field::UpdateTile(int x, int y)
{
    ApplyGravity(x, y);
}

void Field::Update(float dt)
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
}

void Field::Render()
{
    SDL_Rect changedRect = m_updateArea;
    if (m_needUpdateTexture)
    {
        SDL_UpdateTexture(m_texture, nullptr, m_textureColors, m_width * sizeof(Uint32));
        // SDL_UpdateTexture(m_texture, &m_updateArea, m_textureColors, m_width * sizeof(Uint32));
        m_updateArea = {-1, -1, 0, 0};
        m_needUpdateTexture = false;
    }
    
    SDL_RenderCopy(m_renderer, m_texture , nullptr, nullptr);
    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(m_renderer, &changedRect);
}

Uint32 Field::GetColor(const ParticleType type) const
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
    return ColToUint(color);
}

void Field::SwapParticles(const int i1, const int i2) 
{
    const ParticleType temp = m_particles[i1];
    
    m_particles[i1] = m_particles[i2];
    m_particles[i2] = temp;

    int x1, y1, x2, y2;
    Coord(i1, x1, y1);
    Coord(i2, x2, y2);
    std::vector<Pixel> vec= {{x1,y1, GetColor(m_particles[i1])},{x2, y2, GetColor(m_particles[i2])}};
    UpdateTexture(vec);
}

bool Field::CanSwap(int i1, int i2) const
{
    if (m_particles[i2] == ParticleType::None) return true;
    
    if (m_particles[i1] == ParticleType::Sand && m_particles[i2] == ParticleType::Water) return true;

    return false;
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

void Field::UpdateTexture(const std::vector<Pixel>& pixels)
{
    m_needUpdateTexture = true;
    for (const auto pixel : pixels)
    {
        m_textureColors[Ind(pixel.x, pixel.y)] = pixel.color;
        if (m_updateArea.x == -1 && m_updateArea.y == -1)
        {
            m_updateArea = {pixel.x, pixel.y, 1, 1};
            continue;
        } 
        if (pixel.x < m_updateArea.x)
        {
            m_updateArea.w += m_updateArea.x - pixel.x;
            m_updateArea.x = pixel.x;
        }
        if (pixel.x > m_updateArea.x + m_updateArea.w)
        {
            m_updateArea.w = pixel.x - m_updateArea.x;
        }
        if (pixel.y < m_updateArea.y)
        {
            m_updateArea.y = pixel.y;
        }
        if (pixel.y > m_updateArea.y + m_updateArea.h)
        {
            m_updateArea.h = pixel.y - m_updateArea.y;
        }
    }
}


