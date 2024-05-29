//
//  Histogram.h
//  Core
//
//  Created by Rasmus Anthin on 2022-11-22.
//

#pragma once
#include "TextBox.h"


constexpr std::size_t operator "" _sz(unsigned long long n) { return n; }

namespace hist
{

  template<typename T>
  struct Buck
  {
    std::vector<T> samples;
    T start = static_cast<T>(0);
    T end = static_cast<T>(0);
  };

  template<typename T>
  class Histogram
  {
    std::vector<Buck<T>> buckets;
    std::vector<T> samples;
    T range_start = static_cast<T>(0);
    T range_end = static_cast<T>(0);
    size_t num_buckets = 0;
    
    void add_to_bucket(T s)
    {
      auto t = math::value_to_param<float>(s, range_start, range_end);
      auto param = t * static_cast<float>(num_buckets);
      auto b_idx = static_cast<int>(param);
      auto& buck = buckets[b_idx];
      if (s < buck.start && b_idx > 0)
        buckets[b_idx - 1].samples.emplace_back(s);
      else if (s > buck.end && b_idx < num_buckets - 1)
        buckets[b_idx + 1].samples.emplace_back(s);
      else
        buck.samples.emplace_back(s);
    }
    
    void rebuild()
    {
      T step = (range_end - range_start)/static_cast<T>(num_buckets);
      T val_curr = range_start;
      T val_prev = range_start;
      int buck_idx = 0;
      for (auto& buck : buckets)
      {
        val_prev = val_curr;
        val_curr = val_curr + step;
        if (buck_idx++ == num_buckets - 1 && std::abs(val_curr - range_end) < 1e-4f)
          val_curr = range_end;
        buck.start = val_prev;
        buck.end = val_curr;
      }
      
      auto num_samples = samples.size();
      for (size_t s_idx = 0; s_idx < num_samples; ++s_idx)
      {
        auto s = samples[s_idx];
        add_to_bucket(s);
      }
    }
    
  public:
    Histogram(size_t N_buck, T start, T end)
      : num_buckets(std::max(N_buck, 1_sz)), range_start(start), range_end(end)
    {
      buckets.resize(num_buckets);
      
      rebuild();
    }
    
    void operator+=(T val)
    {
      samples.emplace_back(val);
      add_to_bucket(val);
    }
    
    void resize(size_t N_buck, T start, T end)
    {
      num_buckets = std::max(N_buck, 1_sz);
      range_start = start;
      range_end = end;

      buckets.clear();
      buckets.resize(num_buckets);
      
      rebuild();
    }
    
    TextBox to_textbox(int nr, int nc)
    {
      auto hist = *this;
      hist.resize(nc, range_start, range_end);
      size_t max_num_samples = 0;
      for (const auto& buck : hist.buckets)
        math::maximize(max_num_samples, buck.samples.size());
      std::vector<int> hist_bars;
      hist_bars.resize(nc);
      for (int b_idx = 0; b_idx < hist.num_buckets; ++b_idx)
      {
        const auto& buck = hist.buckets[b_idx];
        auto t = static_cast<float>(buck.samples.size())/max_num_samples;
        hist_bars[b_idx] = std::round(t * nr);
      }
      TextBox tb(nr);
      for (int r_idx = nr - 1; r_idx >= 0; --r_idx)
      {
        auto& str = tb[r_idx];
        for (int c_idx = 0; c_idx < nc; ++c_idx)
        {
          if (nr - 1 - r_idx < hist_bars[c_idx])
            str += '#';
          else
            str += ' ';
        }
      }
      return tb;
    }
    
    // Returns number of samples in the wrong bucket.
    int sanity_check_bucket_samples() const
    {
      int num_outside = 0;
      for (const auto& buck : buckets)
      {
        for (const auto& s : buck.samples)
          if (!math::in_range<T>(s, buck.start, buck.end, Range::Closed))
            num_outside++;
      }
      return num_outside;
    }
  };

}
