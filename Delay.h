//
//  Delay.h
//  Terminal Text Lib
//
//  Created by Rasmus Anthin on 2023-11-30.
//  Cloned 2024-03-09 from the Terminal Text Lib.
//  I'm attempting to move it from Terminal Text Lib.
//

#pragma once
#include "Math.h"
#include <thread>
#include <functional>


namespace Delay
{
  // Arguments:
  // int us : microseconds.
  template<typename T>
  void sleep(T us)
  {
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(us)));
  }

  void update_loop(float fps, std::function<bool(void)> update_func)
  {
    std::chrono::milliseconds frame_time(math::roundI(1000 / fps));
    auto last_time = std::chrono::steady_clock::now();

    while (true)
    {
      if (!update_func())
        return;

      // Check time elapsed since the last frame.
      auto current_time = std::chrono::steady_clock::now();
      auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time);

      if (elapsed_time < frame_time)
      {
        // Sleep to limit the refresh rate.
        std::this_thread::sleep_for(frame_time - elapsed_time);
      }

      //refresh();
      //Delay::sleep(delay);

      last_time = current_time;
    }
  }
}
