//
//  Utils.h
//  Core Lib
//
//  Created by Rasmus Anthin on 2024-05-20.
//


#pragma once

#define UNUSED(x) 

namespace utils
{
  
  constexpr int operator"" _i(long double x)
  {
    return static_cast<int>(x);
  }

  template<typename T>
  bool try_set(T* dst, const T& src)
  {
    if (dst != nullptr)
    {
      *dst = src;
      return true;
    }
    return false;
  }
  
  template<typename T>
  T try_get(T* ptr, const T def_val = static_cast<T>(0))
  {
    if (ptr == nullptr)
      return def_val;
    return *ptr;
  }

}
