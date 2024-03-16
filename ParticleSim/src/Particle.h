#pragma once
#include "IVec2.h"
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
    static constexpr Uint8 NONE             = 0b00000000;
    static constexpr Uint8 IMMOVABLE        = 0b00000001;
    static constexpr Uint8 MOVE_VERTICAL    = 0b00000010;
    static constexpr Uint8 MOVE_HORIZONTAL  = 0b00000100;
    
    static constexpr Uint8 IS_LIQUID        = 0b10000000;
    static constexpr Uint8 IS_GAS           = 0b01000000;
    static constexpr Uint8 IS_SOLID         = 0b11000000;
    static constexpr Uint8 TYPE_MASK        = 0b11000000;
};

struct Particle
{
    ParticleType type;
    IVec2 position;
    Vec2 velocity;
    bool isActive = true;
    bool isGrounded;

    bool operator==(const Particle& other) const;
};

namespace ParticleDefinitions
{
    Uint32 GetColorByType(ParticleType type);
    int GetMassByType(ParticleType type);
    Uint8 GetActionsByType(ParticleType type);
    bool CanBeMoved(Uint8);
    // mb should be not here
    bool HasAction(Uint8 actions, const Uint8 actionToCheck);
    bool IsGas(Uint8 actions);
    bool IsLiquid(Uint8 actions);
    bool IsSolid(Uint8 actions);
}
