#pragma once
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
    static constexpr Uint8 NONE = 0;
    static constexpr Uint8 FALL = 1 << 0;
    static constexpr Uint8 RISE = 1 << 1;
    static constexpr Uint8 SIDES = 1 << 2;
    static constexpr Uint8 IMMOVABLE = 1 << 3;
};

struct Particle
{
    ParticleType type;
    Uint8 Actions;
    bool isActive;
    bool frameToUpdateFlag;
    
};

namespace ParticleDefenitions
{
    Uint32 GetColorByType(const ParticleType type);
    Uint8 GetActionsByType(ParticleType type);
    int GetParticleDensity(ParticleType type);
    
}
