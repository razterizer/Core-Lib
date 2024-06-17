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
  operator bool() const { return val; }
  operator bool&() { return val; }
};

using bool_vector = std::vector<Bool>;
