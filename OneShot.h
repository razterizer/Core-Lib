//
//  OneShot.h
//  Core
//
//  Created by Rasmus Anthin.
//

#pragma once

class OneShot
{
  bool val = true;
  
public:
  
  OneShot() = default;
  OneShot(bool preset) : val(preset) {}
  
  bool once()
  {
    bool curr = val;
    val = false;
    return curr;
  }
  
  bool was_triggered() const { return !val; }
  
  void reset()
  {
    val = true;
  }
  
};
