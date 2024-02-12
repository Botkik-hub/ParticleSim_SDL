#pragma once
#include "SDL_render.h"

class Filed;

class TheGame
{
public:
    static const int WIDTH = 800;
    static const int HEIGHT = 600;
    
private:
    bool isRunning;
    SDL_Renderer* m_renderer;

    Filed* m_filed;
    
public:

    static TheGame& Instance();
    SDL_Renderer* GetRenderer() const;
    
    void Run();

private:
    void Init();

    void HandleEvents();
    void Update(float dt);
    void Render();

    void CleanUp();
};
