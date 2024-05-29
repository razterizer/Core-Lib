//
//  Math.h
//  Core
//
//  Created by Rasmus Anthin.
//

#pragma once
#include <cstdlib>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <optional>
#include <type_traits>
#ifdef _WIN32
#ifdef _MSC_VER
#include <corecrt_math_defines.h>
#endif
#endif

enum class Range { Free, Closed, Open, ClosedOpen, OpenClosed, ClosedFree, FreeClosed, OpenFree, FreeOpen };

template<typename T>
struct RangeData
{
  std::optional<T> start = {};
  std::optional<T> end = {};
  Range range = Range::Free;
  
  RangeData(std::optional<T> start_val = {}, std::optional<T> end_val = {}, Range range_type = Range::Free)
   : start(start_val), end(end_val), range(range_type)
  {}
};

namespace math
{

  static constexpr float c_pi = static_cast<float>(M_PI);
  static constexpr float c_2pi = static_cast<float>(2*M_PI);
  static constexpr float c_sqrt2 = static_cast<float>(M_SQRT2);
  static constexpr float c_2_sqrt_pi = static_cast<float>(M_2_SQRTPI);
  static constexpr float c_sqrt1_2 = static_cast<float>(M_SQRT1_2);
  static constexpr float c_ln2 = static_cast<float>(M_LN2);
  static constexpr float c_e = static_cast<float>(M_E);
  static constexpr float c_1_e = static_cast<float>(1/M_E);

  // ////////////////

  template<typename T>
  int roundI(T val)
  {
    return static_cast<int>(std::round(val));
  }

  template<typename T>
  int sgn(T val)
  {
    if (val < 0)
      return -1;
    if (val > 0)
      return +1;
    return 0;
  }
  
  template<typename T>
  int sign(T val)
  {
    if (val < 0)
      return -1;
    return +1;
  }
  
  template<typename T>
  T sq(T val)
  {
    return val * val;
  }

  template<typename T>
  T cube(T val)
  {
    return val * val * val;
  }
  
  template<typename T>
  T length_squared(T x, T y)
  {
    return sq(x) + sq(y);
  }
  
  template<typename T>
  T length(T x, T y)
  {
    return std::sqrt(length_squared(x, y));
  }

  template<typename T>
  T distance_squared(T x0, T y0, T x1, T y1)
  {
    auto diff_x = x1 - x0;
    auto diff_y = y1 - y0;
    return length_squared(diff_x, diff_y);
  }

  template<typename T>
  T distance(T x0, T y0, T x1, T y1)
  {
    return std::sqrt(distance_squared(x0, y0, x1, y1));
  }
  
  template<typename T>
  bool normalize(T& x, T& y)
  {
    auto len = length(x, y);
    if (len < 1e-10f)
      return false;
    x /= len;
    y /= len;
    return true;
  }
  
  template<typename T>
  T clamp(T val, T lo, T hi)
  {
    if (val < lo)
      return lo;
    if (hi < val)
      return hi;
    return val;
  }
  
  template<typename T>
  T lerp(float t, T a, T b)
  {
    return static_cast<T>(a*(1.f-t) + b*t);
  }
  
  template<typename T>
  float value_to_param(T val, T start, T end)
  {
    if (start == end)
    {
      if (val < start)
        return 0.f;
      else
        return 1.f;
    }
    return static_cast<float>(val - start) / static_cast<float>(end - start);
  }

  template<typename T>
  float value_to_param_clamped(T val, T start, T end)
  {
    //val = clamp(val, start, end);
    auto t = value_to_param(val, start, end);
    t = clamp(t, 0.f, 1.f);
    return t;
  }
  
  template<typename T>
  std::vector<T> linspace(T start, T step, T end)
  {
    std::vector<T> res;
    if (step > 0)
      for (T v = start; v <= end; v += step)
        res.emplace_back(v);
    else if (step < 0)
      for (T v = start; v >= end; v += step)
        res.emplace_back(v);
    return res;
  }
  
  template<typename T0, typename T1>
  T1 linmap(T0 val_0, T0 start_0, T0 end_0, T1 start_1, T1 end_1)
  {
    auto t = value_to_param<T0>(val_0, start_0, end_0);
    return lerp<T1>(t, start_1, end_1);
  }

  template<typename T>
  bool in_range(T val, std::optional<T> start, std::optional<T> end, Range type)
  {
    switch (type)
    {
      case Range::Free:
        return true;
      case Range::Closed:
        return start.value_or(val) <= val && val <= end.value_or(val);
      case Range::Open:
        return start.value_or(val-1) < val && val < end.value_or(val+1);
      case Range::ClosedOpen:
        return start.value_or(val) <= val && val < end.value_or(val+1);
      case Range::OpenClosed:
        return start.value_or(val-1) < val && val <= end.value_or(val);
      case Range::ClosedFree:
        return start.value_or(val) <= val;
      case Range::FreeClosed:
        return val <= end.value_or(val);
      case Range::OpenFree:
        return start.value_or(val-1) < val;
      case Range::FreeOpen:
        return val < end.value_or(val+1);
    }
    return false;
  }

  template<typename T>
  bool in_range(T val, const RangeData<T>& rd)
  {
    return in_range(val, rd.start, rd.end, rd.range);
  }

  template<typename T>
  bool in_unit_range(T val, Range type)
  {
    return in_range(val, static_cast<T>(0), static_cast<T>(1), type);
  }

  template<typename T>
  bool minimize(T& val, T test, const RangeData<T>& rd = {})
  {
    if (test < val && in_range(test, rd))
    {
      val = test;
      return true;
    }
    return false;
  }

  template<typename T>
  bool maximize(T& val, T test, const RangeData<T>& rd = {})
  {
    if (test > val && in_range(test, rd))
    {
      val = test;
      return true;
    }
    return false;
  }

  template<typename T>
  T get_max() { return std::is_floating_point_v<T> ? std::numeric_limits<T>::infinity() : std::numeric_limits<T>::max(); }

  template<typename T>
  T get_min() { return std::is_floating_point_v<T> ? -std::numeric_limits<T>::infinity() : std::numeric_limits<T>::min(); }

  template<typename T>
  bool is_eps(T v, T eps = std::numeric_limits<T>::epsilon())
  {
    return std::abs(v) < eps;
  }

  template<typename T>
  bool fuz_eq(T a, T b, T eps = std::numeric_limits<T>::epsilon())
  {
    return is_eps(a - b, eps);
  }
  
  template<typename T>
  T deg2rad(T deg) { return static_cast<T>(M_PI / 180.) * deg; }
  
  template<typename T>
  T rad2deg(T rad) { return static_cast<T>(180. / M_PI) * rad; }
  
  // Function to calculate the GCD of two numbers using Euclidean algorithm
  int gcd(int a, int b)
  {
    while (b != 0)
    {
      int temp = b;
      b = a % b;
      a = temp;
    }
    return a;
  }
  
  float gcd(float a, float b)
  {
    // Ensure non-negative values for the algorithm
    a = std::abs(a);
    b = std::abs(b);
    
    while (b > 0.001)  // Using a small threshold for precision
    {
      float temp = std::fmod(a, b);
      a = b;
      b = temp;
    }
    
    return a;
  }
}
