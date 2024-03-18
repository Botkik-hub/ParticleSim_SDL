#pragma once
#include "IVec2.h"
#include "SDL_render.h"

class FieldChunk;

class TheGame
{
public:
    static constexpr int WIDTH = 200;
    static constexpr int HEIGHT = 150;

    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;
    
private:
    bool m_isRunning = false;
    SDL_Renderer* m_renderer = nullptr;

    FieldChunk* m_filed = nullptr;

    float m_deltaTime = 0;

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
    void Update() const;
    void Render() const;

    void CleanUp() const;
};
