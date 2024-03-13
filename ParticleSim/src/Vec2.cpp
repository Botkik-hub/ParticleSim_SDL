#include "Vec2.h"

#include "IVec2.h"

Vec2 Vec2::operator+(const Vec2& other) const
{
    Vec2 result;
    result.x = this->x + other.x;
    result.y = this->y + other.y;
    return result;
}

Vec2 Vec2::operator-(const Vec2& other) const
{
    Vec2 result;
    result.x = this->x - other.x;
    result.y = this->y - other.y;
    return result;
}

Vec2 Vec2::operator*(const float other) const
{
    Vec2 result;
    result.x = this->x * other;
    result.y = this->y * other;
    return result;
}

Vec2 Vec2::operator/(const float other) const
{
    Vec2 result;
    result.x = this->x / other;
    result.y = this->y / other;
    return result;
}

bool Vec2::operator==(const Vec2& other) const
{
    return other.x == x && other.y == y;  // NOLINT(clang-diagnostic-float-equal)
}

Vec2::operator IVec2() const
{
    return {static_cast<int>(x), static_cast<int>(y)};
}

float Vec2::SquareMagnitude() const
{
    return x*x + y+y;
}

Vec2::Vec2()
= default;

Vec2::Vec2(const float x, const float y): x(x), y(y)
{
}
