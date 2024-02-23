#pragma once
#include "IVec2.h"
#include "SDL_stdinc.h"

enum class ParticleType
{
    None,
    Sand,
    Water,
    Steam,
    Stone
};

class ParticleAction
{
public:
    static const Uint8 None = 0;
    static const Uint8 Immovable = 1 << 0;
    static const Uint8 MoveDown = 1 << 1;
    static const Uint8 MoveSides = 1 << 2;
    static const Uint8 MoveUp = 1 << 3;
};
struct Particle
{
    ParticleType type;
    Uint8 actions;
    IVec2 velocity;
    bool isActive;
    bool frameToUpdateFlag;
};

namespace ParticleDefinitions
{
    Uint32 GetColorByType(const ParticleType type);
    int GetDensityByType(const ParticleType type);
    Uint8 GetActionsByType(const ParticleType type);
    bool CanBeMoved(const Uint8);
}
