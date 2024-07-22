//
//  Utils.h
//  Core Lib
//
//  Created by Rasmus Anthin on 2024-05-20.
//


#pragma once

namespace utils
{

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

}
