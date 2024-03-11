#pragma once
#include "SDL_stdinc.h"
#include "Vec2.h"

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
    static constexpr Uint8 MOVE_VERTICAL = 1 << 4;
    static constexpr Uint8 MOVE_HORIZONTAL = 1 << 5;
};
struct Particle
{
    ParticleType type;
    Uint8 actions;
    Vec2 velocity;
    bool isActive;
    bool frameToUpdateFlag;
    union
    {
        // not used for now can have properties of other stuff
    };
};

namespace ParticleDefinitions
{
    Uint32 GetColorByType(ParticleType type);
    int GetMassByType(ParticleType type);
    Uint8 GetActionsByType(ParticleType type);
    bool CanBeMoved(Uint8);
    
    // mb should be not here
    bool HasAction(Uint8 actions, const Uint8 actionToCheck);
}
