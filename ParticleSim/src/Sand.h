#pragma once
#include "Particle.h"

class Sand : Particle
{
public:
    Sand(Field* field);


    virtual void Update() override;
private:
    void Move();
};
