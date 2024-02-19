#pragma once

struct IVec2
{
   int x;
   int y;

   IVec2 operator+(const IVec2& other) const;
   IVec2 operator-(const IVec2& other) const;
   IVec2 operator*(float other) const;
   IVec2 operator/(const float other) const;
   IVec2 operator*(int other) const;
   IVec2 operator/(int other) const;
};
