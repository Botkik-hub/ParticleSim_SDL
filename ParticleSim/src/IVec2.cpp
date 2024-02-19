#include "IVec2.h"

IVec2 IVec2::operator+(const IVec2& other) const
{
    IVec2 result;
    result.x = this->x + other.x;
    result.y = this->y + other.y;
    return result;
}

IVec2 IVec2::operator-(const IVec2& other) const
{
    IVec2 result;
    result.x = this->x - other.x;
    result.y = this->y - other.y;
    return result;
}

IVec2 IVec2::operator*(const float other) const
{
    IVec2 result;
    result.x = this->x * other;
    result.y = this->y * other;
    return result;
}

IVec2 IVec2::operator/(const float other) const
{
    IVec2 result;
    result.x = this->x / other;
    result.y = this->y / other;
    return result;
}

IVec2 IVec2::operator*(const int other) const
{
    IVec2 result;
    result.x = this->x * other;
    result.y = this->y * other;
    return result;
}

IVec2 IVec2::operator/(const int other) const
{
     IVec2 result;
     result.x = this->x / other;
     result.y = this->y / other;
     return result;
 }
