#pragma once

class Field;

class Particle
{
public:
    explicit Particle(Field* field) : x(0), y(0), m_field(field){}

    virtual ~Particle() = default;
    
    int x;
    int y;
    
    bool needUpdate = true;

protected:
    Field* m_field;

public:
    virtual void Update();
};
