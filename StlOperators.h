//
//  StlOperators.h
//  Core
//
//  Created by Rasmus Anthin on 2024-05-29.
//

#pragma once

namespace stloperators
{
  
  template<typename T>
  std::optional<T> operator* (const std::optional<T>& A, const std::optional<T>& B)
  {
    if (A.has_value() && B.has_value())
      return A.value() * B.value();
    return std::nullopt;
  }
  
  template<typename T>
  std::optional<T> operator* (const std::optional<T>& A, const T& B)
  {
    if (A.has_value())
      return A.value() * B;
    return std::nullopt;
  }
  
  template<typename T>
  std::optional<T> operator* (const T& A, const std::optional<T>& B)
  {
    if (B.has_value())
      return A * B.value();
    return std::nullopt;
  }
  
}
