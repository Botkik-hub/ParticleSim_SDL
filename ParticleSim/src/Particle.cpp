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
         return ParticleAction::MOVE_DOWN;
    case ParticleType::Water:
         return ParticleAction::MOVE_DOWN | ParticleAction::MOVE_SIDES;
    case ParticleType::Steam:
         return ParticleAction::MOVE_UP | ParticleAction::MOVE_SIDES;
    case ParticleType::Stone:
         return ParticleAction::IMMOVABLE;
     default:
         throw std::exception("type is missing");
     }
}

bool ParticleDefinitions::CanBeMoved(const Uint8 actions)
{
    return (actions & ParticleAction::IMMOVABLE) != ParticleAction::IMMOVABLE;
}
