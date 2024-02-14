#pragma once

enum class ParticleType
{
    None,
    Sand,
    Water
};


struct Particle
{
    ParticleType type;
    bool isActive;
    bool frameToUpdateFlag;
};
