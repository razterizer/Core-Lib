//
//  Benchmark.h
//  Core
//
//  Created by Rasmus Anthin on 2024-10-31.
//

#pragma once
#include "StringHelper.h"
#include <chrono>
#include <map>


#define BM_FUNC(f) [&](){ f; }, #f

namespace benchmark
{
  
  template<typename Lambda>
  float calc_time_ms(Lambda&& func)
  {
    auto start_time = std::chrono::steady_clock::now();
    func();
    auto end_time = std::chrono::steady_clock::now();
    
    std::chrono::duration<float, std::milli> elapsed_time = end_time - start_time;
    return elapsed_time.count();
  }
  
  std::chrono::time_point<std::chrono::steady_clock> benchmark_tictoc_time;
  
  void tic()
  {
    benchmark_tictoc_time = std::chrono::steady_clock::now();
  }
  
  float toc()
  {
    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> elapsed_time = end_time - benchmark_tictoc_time;
    return elapsed_time.count();
  }
  
  class Benchmark
  {
    std::map<std::string, float> timers_ms;
    std::map<std::string, std::chrono::steady_clock::time_point> start_times;
    
    void print()
    {
      int max_tag_len = 0;
      int max_time_len = 0;
      for (const auto& [tag, time_ms] : timers_ms)
      {
        math::maximize(max_tag_len, static_cast<int>(tag.size()));
        math::maximize(max_time_len, static_cast<int>(std::to_string(time_ms).size()));
      }
      
      std::vector<std::string> lines;
      for (const auto& [tag, time_ms] : timers_ms)
        lines.emplace_back(str::adjust_str(tag, str::Adjustment::Left, max_tag_len)
          + " : "
          + str::adjust_str(std::to_string(time_ms), str::Adjustment::Left, max_time_len)
          + " ms");
      
      // ##################
      // #   Benchmark    #
      // ##################
      // # .............. #
      // # .............. #
      // ##################
      
      if (lines.empty())
        return;
      auto frame_width = static_cast<int>(lines.back().size() + 4);
      auto horiz_str = str::rep_char('#', frame_width);
      std::cout << horiz_str << std::endl;
      std::cout << "# " << str::adjust_str("BENCHMARK", str::Adjustment::Center, frame_width - 3) << " #" << std::endl;
      std::cout << horiz_str << std::endl;
      for (const auto& l : lines)
        std::cout << "# " << l << " #" << std::endl;
      std::cout << horiz_str << std::endl;
    }
    
  public:
    ~Benchmark()
    {
      print();
    }
    
    template<typename Lambda>
    void reg(Lambda&& func, const std::string& tag)
    {
      timers_ms[tag] += calc_time_ms(func);
    }
    
    void start(const std::string& tag)
    {
      start_times[tag] = std::chrono::steady_clock::now();
    }
    
    void stop(const std::string& tag)
    {
      auto end_time = std::chrono::steady_clock::now();
      std::chrono::duration<float, std::milli> elapsed_time = end_time - start_times[tag];
      timers_ms[tag] += elapsed_time.count();
    }
  };
  
}
