#pragma once

struct IVec2;

struct Vec2
{
   float x;
   float y;

   Vec2 operator+(const Vec2& other) const;
   Vec2 operator-(const Vec2& other) const;
   Vec2 operator*(float other) const;
   Vec2 operator/(const float other) const;

   bool operator==(const Vec2& other) const;
   
   operator IVec2() const;

   Vec2();
   Vec2(float x, float y);
};
