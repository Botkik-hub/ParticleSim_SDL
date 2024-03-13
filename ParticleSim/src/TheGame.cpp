#include "TheGame.h"

#include <complex>
#include <cstdio>
#include <iostream>
#include <SDL.h>
#include <iostream>

#include "Field.h"
#include "Particle.h"

TheGame& TheGame::Instance()
{
    static TheGame game;
    return game;
}

SDL_Renderer* TheGame::GetRenderer() const
{
    return m_renderer;
}

void TheGame::Run()
{
    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win;

    SDL_CreateWindowAndRenderer( WINDOW_WIDTH, WINDOW_HEIGHT, 0, &win, &m_renderer);
	
    m_deltaTime = 0.16f;

    Init();

    m_isRunning = true;
    
    while (m_isRunning)
    {
        const Uint32 frameStart = SDL_GetTicks();

        HandleEvents();
        Update();
        Render();

        const Uint32 frameEnd = SDL_GetTicks();
        m_deltaTime = static_cast<float>(frameEnd - frameStart) / 1000.0f;
        if (m_deltaTime  < 0.016)
        {
            const Uint32 time = 16 - (frameEnd - frameStart);
            SDL_Delay(time);
            m_deltaTime = 0.016f;
        }
    }
    CleanUp();
}

float TheGame::GetDeltaTime() const
{
    return m_deltaTime;
}

void TheGame::Init()
{
    m_filed = new Field;
    
    for (int x = 70; x < 100; ++x)
    {
        for (int y = 25; y < 50; ++y)
        {
            m_filed->SpawnParticle(x, y, ParticleType::Sand);
        }
    }
    
    for (int x = 100; x < WIDTH; ++x)
    {
        for (int y = 20; y < 50; ++y)
        {
            m_filed->SpawnParticle(x, y, ParticleType::Water);
        }
    }
    for (int x = 0; x < WIDTH; ++x)
    {
        for (int y = HEIGHT - 1; y >= HEIGHT - 1 - 10; --y)
        {
            m_filed->SpawnParticle(x, y, ParticleType::Steam);
        }
    }
    for (int x = 50; x < 100; ++x)
    {
        for (int y = 90; y < 110; ++y)
        {
            m_filed->SpawnParticle(x, y, ParticleType::Stone);
        }
    }
}

void TheGame::SpawnParticlesInLine(IVec2 start, IVec2 end) const
{
    const int dx = std::abs(end.x - start.x);
    const int dy = -std::abs(end.y - start.y);

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

    while (true)
    {
        m_filed->SpawnParticle(start.x, start.y, ParticleType::Stone);

        if (start == end) break;

        const int error2 = error + error;
        if (error2 >= dy)
        {
            if (start.x == end.x) break;

            error = error + dy;
            start.x = start.x + s.x;
        }
        if (error2 <= dx)
        {
            if (start.y == end.y) break;

            error = error + dx;
            start.y = start.y + s.y;
        }
    }
}

void TheGame::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_MOUSEBUTTONDOWN:
            OnMouseClick();
            break;
        case SDL_QUIT:
            m_isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                m_isRunning = false;
            break;
        default:
            break;
        }
    }
}

void TheGame::OnMouseClick()
{
    int x ;
    int y;
    SDL_GetMouseState(&x, &y);
    x = x * WIDTH / WINDOW_WIDTH;
    y = y * HEIGHT / WINDOW_HEIGHT;
    // if (m_wasClicked)
    // {
    //     SpawnParticlesInLine({x , y}, m_mousePos);
    //     m_wasClicked = false;
    // }
    // else
    // {
    //     m_mousePos = {x, y};
    //     m_wasClicked = true;
    // }
    Particle* particle = m_filed->GetParticleAtPosition({x, y});
    if (particle == nullptr)
    {
        std::cout << "NULL" << '\n';
    }else
    {
        std::cout << "Type " << (int)particle->type << " Position : " << particle->position.x << ", " << particle->position.y
        << " Velocity : " << particle->velocity.x << ", " << particle->velocity.y << "\n";
    }
    std::cout.flush();
}

void TheGame::Update() const
{
    static int count = 0;
    if (count < 300)
    {
        m_filed->SpawnParticle(100, 20, ParticleType::Sand);
        count++;
    }
    m_filed->Update();
}

void TheGame::Render() const
{
    SDL_RenderClear(m_renderer);
    
    m_filed->Render();
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 1);
    SDL_RenderPresent(m_renderer);
}

void TheGame::CleanUp() const
{
    delete m_filed;
}
