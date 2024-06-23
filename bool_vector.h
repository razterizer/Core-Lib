//
//  bool_vector.h
//  Core
//
//  Created by Rasmus Anthin on 2024-06-17.
//

#pragma once
#include <vector>

class Bool
{
  bool val = false;

public:
  Bool() = default;
  Bool(bool v) : val(v) {}
  Bool(const Bool& b) : val(b.val) {}
  Bool(int v) : val(static_cast<bool>(v)) {}

  operator bool() const { return val; }
  operator bool&() { return val; }
  
  bool operator==(Bool b) const { return val == b.val; }
  bool operator!=(Bool b) const { return val == b.val; }
};

using bool_vector = std::vector<Bool>;
