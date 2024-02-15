#include "Particle.h"


Uint32 ParticleDefenitions::GetColorByType(const ParticleType type)
{
    switch (type) {
    case ParticleType::None:
        return 0;
        break;
    case ParticleType::Sand:
        return 255u << 24 | 255u << 16 | 0u << 8 | 255;
        break;
    case ParticleType::Water:
        return 0u << 24 | 0u << 16 | 255u << 8 | 255;
        break;
    case ParticleType::Steam:
        return 255u << 24 | 255u << 16 | 255u << 8 | 255;
        break;
    case ParticleType::Stone:
        return 128u << 24 | 128u << 16 | 128u << 8 | 255;
        break;
    }
    return 0;
}



Uint8 ParticleDefenitions::GetActionsByType(const ParticleType type)
{
    switch (type) {
    case ParticleType::None:
        return 0;
        break;
    case ParticleType::Sand:
        return ParticleAction::FALL;
        break;
    case ParticleType::Water:
        return ParticleAction::FALL | ParticleAction::SIDES;
        break;
    case ParticleType::Steam:
        return ParticleAction::RISE | ParticleAction::SIDES;
        break;
    case ParticleType::Stone:
        return ParticleAction::IMMOVABLE;
        break;
    }
    return 0;
}

int ParticleDefenitions::GetParticleDensity(const ParticleType type)
{
    switch (type) {
    case ParticleType::None:
        return 0;
        break;
    case ParticleType::Sand:
        return 5;
        break;
    case ParticleType::Water:
        return 1;
        break;
    case ParticleType::Steam:
        return -1;
        break;
    case ParticleType::Stone:
        return 99;
        break;
    }
    return 0;
}