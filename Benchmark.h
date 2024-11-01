//
//  Benchmark.h
//  Core
//
//  Created by Rasmus Anthin on 2024-10-31.
//

#pragma once
#include <chrono>
#include <string>
#include <map>


#define BM_FUNC(f) [&](){ f; }, #f

template<typename Lambda>
float calc_time_ms(Lambda&& func)
{
  auto start_time = std::chrono::steady_clock::now();
  func();
  auto end_time = std::chrono::steady_clock::now();
  
  std::chrono::duration<float, std::milli> elapsed_time = end_time - start_time;
  return elapsed_time.count();
}

class Benchmark
{
  std::map<std::string, float> timers_ms;
  std::map<std::string, std::chrono::steady_clock::time_point> start_times;
  
public:
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
  
  void print()
  {
    for (const auto& [tag, time_ms] : timers_ms)
      std::cout << tag << ": " << time_ms << " ms" << std::endl;
  }
};
