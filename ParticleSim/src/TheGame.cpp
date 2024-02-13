#include "TheGame.h"
#include <cstdio>
#include <iostream>
#include <SDL.h>
#include <iostream>

#include "Field.h"

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

    SDL_CreateWindowAndRenderer( 800, 600, 0, &win, &m_renderer);

	
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
    m_filed = new Field;
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
    m_filed->Update(dt);
    std::cout << "delta time: " << dt << '\n';
}

void TheGame::Render()
{
    SDL_RenderClear(m_renderer);
    //SDL_Rect r = {100, 100, 100, 100};
    //SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    //SDL_RenderDrawRect(m_renderer, &r);
    
    m_filed->Render();
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 1);
    SDL_RenderPresent(m_renderer);
}

void TheGame::CleanUp()
{
    delete m_filed;
}
