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
    static constexpr Uint8 NONE = 0;
    static constexpr Uint8 IMMOVABLE = 1 << 0;
    static constexpr Uint8 MOVE_DOWN = 1 << 1;
    static constexpr Uint8 MOVE_SIDES = 1 << 2;
    static constexpr Uint8 MOVE_UP = 1 << 3;
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
    Uint32 GetColorByType(ParticleType type);
    int GetMassByType(ParticleType type);
    Uint8 GetActionsByType(ParticleType type);
    bool CanBeMoved(Uint8);
}
