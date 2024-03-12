﻿#pragma once
#include "IVec2.h"
#include "SDL_render.h"

class Field;

class TheGame
{
public:
    static const int WIDTH = 200;
    static const int HEIGHT = 150;

    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 600;
    
private:
    bool isRunning;
    SDL_Renderer* m_renderer;

    Field* m_filed;

    float m_deltaTime;

    IVec2 m_mousePos;
    bool m_wasClicked = false;
public:

    static TheGame& Instance();
    SDL_Renderer* GetRenderer() const;
    
    void Run();

    float GetDeltaTime() const;
private:
    void Init();
    void SpawnParticlesInLine(IVec2 start, IVec2 end) const;

    void HandleEvents();
    void OnMouseClick();
    void Update();
    void Render();

    void CleanUp();
};
