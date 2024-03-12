#include "Particle.h"

#include <exception>

Uint32 ParticleDefinitions::GetColorByType(const ParticleType type)
{
    switch (type) {
    case ParticleType::None:
        return 0;
    case ParticleType::Sand:
        return 255u << 24 | 255u << 16 | 0u << 8 | 255;
    case ParticleType::Water:
        return 0u << 24 | 0u << 16 | 255u << 8 | 255;
    case ParticleType::Steam:
        return 255u << 24 | 255u << 16 | 255u << 8 | 255;
    case ParticleType::Stone:
        return 128u << 24 | 128u << 16 | 128u << 8 | 255;
    default:
        throw std::exception("type is missing");
    }
}

int ParticleDefinitions::GetMassByType(const ParticleType type)
{
    switch (type) {
    case ParticleType::None:
        return 0;
    case ParticleType::Sand:
        return 5;
    case ParticleType::Water:
        return 1;
    case ParticleType::Steam:
        return -1;
    case ParticleType::Stone:
        return 99;
    default:
        throw std::exception("type is missing");
    }
}

Uint8 ParticleDefinitions::GetActionsByType(const ParticleType type)
{
    switch (type) {
    case ParticleType::None:
         return ParticleAction::NONE;
    case ParticleType::Sand:
         return ParticleAction::MOVE_VERTICAL | ParticleAction::IS_SOLID;
    case ParticleType::Water:
         return ParticleAction::MOVE_VERTICAL | ParticleAction::MOVE_HORIZONTAL | ParticleAction::IS_LIQUID;
    case ParticleType::Steam:
         return ParticleAction::MOVE_VERTICAL | ParticleAction::MOVE_HORIZONTAL | ParticleAction::IS_GAS;
    case ParticleType::Stone:
         return ParticleAction::IMMOVABLE | ParticleAction::IS_SOLID;
     default:
         throw std::exception("type is missing");
     }
}


bool ParticleDefinitions::CanBeMoved(const Uint8 actions)
{
    return (actions & ParticleAction::IMMOVABLE) != ParticleAction::IMMOVABLE;
}


bool ParticleDefinitions::HasAction(const Uint8 actions, const Uint8 actionToCheck)
{
    return (actions & actionToCheck) != 0;
}

bool ParticleDefinitions::IsGas(const Uint8 actions)
{
    return (actions & ParticleAction::IS_GAS) == ParticleAction::IS_GAS;
}

bool ParticleDefinitions::IsLiquid(const Uint8 actions)
{
    return (actions & ParticleAction::IS_LIQUID) == ParticleAction::IS_LIQUID;
}

bool ParticleDefinitions::IsSolid(const Uint8 actions)
{
    return (actions & ParticleAction::IS_SOLID) == ParticleAction::IS_SOLID;
}
