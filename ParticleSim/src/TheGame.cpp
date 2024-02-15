#include "TheGame.h"
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

    SDL_CreateWindowAndRenderer( 1600, 1200, 0, &win, &m_renderer);

	
    float dt = 0.0f;

    Init();

    isRunning = true;
    
    while (isRunning)
    {
        const Uint32 frameStart = SDL_GetTicks();

        HandleEvents();
        Update(dt);
        Render();

        const Uint32 frameEnd = SDL_GetTicks();
        dt = static_cast<float>(frameEnd - frameStart) / 1000.0f;
        if (dt < 0.016)
        {
            const Uint32 time = 16 - (frameEnd - frameStart);
            SDL_Delay(time);
            dt = 0.016;
        }
    }
    CleanUp();
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

void TheGame::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                isRunning = false;
            break;
        }
    }
}


void TheGame::Update(float dt)
{
    static int count = 0;
    if (count < 300)
    {
        m_filed->SpawnParticle(100, 20, ParticleType::Sand);
        count++;
    }
    m_filed->Update(dt);
    std::cout << "delta time: " << dt << '\n';
}

void TheGame::Render()
{
    SDL_RenderClear(m_renderer);
    
    m_filed->Render();
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 1);
    SDL_RenderPresent(m_renderer);
}

void TheGame::CleanUp()
{
    delete m_filed;
}
