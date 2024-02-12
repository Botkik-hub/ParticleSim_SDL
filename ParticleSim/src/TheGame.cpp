#include "TheGame.h"
#include <cstdio>
#include <iostream>
#include <SDL.h>
#include <iostream>

ParticleType** TheGame::m_particles = nullptr;
SDL_Renderer* TheGame::m_renderer = nullptr;
bool TheGame::isRunning = false;

void TheGame::Run()
{
    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("GAME",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600, 0);

    m_renderer = SDL_GetRenderer(win);
	
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
    }
    CleanUp();
}

void TheGame::Init()
{
    m_particles = new ParticleType*[WIDTH];
    for (int i = 0; i < WIDTH; ++i)
    {
        m_particles[i] = new ParticleType[HEIGHT];
        for (int j = 0; j < HEIGHT; ++j)
        {
            m_particles[i][j] = ParticleType::None;
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
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            //std::cout << (int)m_particles[x][y] << " ";
        }
        std::cout << '\n';
    }
    std::cout << "delta time: " << dt << '\n';
}

void TheGame::Render()
{
    
}

void TheGame::CleanUp()
{
    for (int i = 0; i < WIDTH; ++i)
    {
        delete m_particles[i];
    }
    delete m_particles;
}
