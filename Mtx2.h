//
//  Mtx2.h
//  Core
//
//  Created by Rasmus Anthin on 2024-10-30.
//

#pragma once
#include "Vec2.h"

// /        \
// | 00  01 |
// | 10  11 |
// \        /

// /        \
// | rr  rc |
// | cr  cc |
// \        /

class Mtx2
{
  float m00 = 0.f;
  float m01 = 0.f;
  float m10 = 0.f;
  float m11 = 0.f;
  
public:
  Mtx2(float v00, float v01, float v10, float v11)
    : m00(v00)
    , m01(v01)
    , m10(v10)
    , m11(v11)
  {}
  
  Vec2 operator*(const Vec2& v)
  {
    Vec2 ret
    {
      m00 * v.r + m01 * v.c,
      m10 * v.r + m11 * v.c
    };
    return ret;
  }
};
