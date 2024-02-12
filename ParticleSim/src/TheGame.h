#pragma once
#include "SDL_render.h"

enum class ParticleType
{
    None,
    Sand,
    Water
};

class TheGame
{
private:
    static const int WIDTH = 800;
    static const int HEIGHT = 600;

    static bool isRunning;
    
    static ParticleType** m_particles;

    static SDL_Renderer* m_renderer;
    
public:

    static void Run();
    
    static void Init();

    static void HandleEvents();
    static void Update(float dt);
    static void Render();

    static void CleanUp();
};
