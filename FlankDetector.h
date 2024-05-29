//
//  FlankDetector.h
//  Core
//
//  Created by Rasmus Anthin.
//

#pragma once

template<typename T = float>
class FlankDetector
{
  T curr_val = static_cast<T>(0);
  T prev_val = static_cast<T>(0);
 
public:
  FlankDetector() = default;
  FlankDetector(T curr, T prev) : curr_val(curr), prev_val(prev) {}

  void update()
  {
    prev_val = curr_val;
  }
  
  void set(T val)
  {
    curr_val = val;
  }
  
  void update(T val)
  {
    update();
    set(val);
  }
  
  bool pos_flank(T threshold = static_cast<T>(0)) const { return curr_val > prev_val + threshold; }
  bool neg_flank(T threshold = static_cast<T>(0)) const { return curr_val < prev_val - threshold; }
  bool curr() const { return curr_val; }
  bool prev() const { return prev_val; }
};
