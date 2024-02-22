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

struct Particle
{
    ParticleType type;
    bool canBeMoved;
    IVec2 velocity;
    bool isActive;
    bool frameToUpdateFlag;
};

namespace ParticleDefinitions
{
    Uint32 GetColorByType(const ParticleType type);
    int GetDensityByType(const ParticleType type);
}
