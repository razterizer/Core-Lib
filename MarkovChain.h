//
//  MarkovChain.h
//  Core
//
//  Created by Rasmus Anthin on 2022-11-15.
//

#pragma once
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Rand.h"

namespace markov_chain
{

  template<typename T>
  class MarkovChain
  {
  public:
    using MarkovChainTransitionTable = std::map<T, std::vector<std::pair<T, float>>>;
    MarkovChainTransitionTable mctt;
    T empty_item;
    
    MarkovChain(T empty_val) : empty_item(empty_val) {}
  
    void add_transition(T from, T to)
    {
      auto& targets = mctt[from];
      auto it = std::find_if(targets.begin(), targets.end(), [&to](const auto& ipair) { return ipair.first == to; });
      if (it != targets.end())
        it->second++;
      else
        targets.emplace_back(to, 1);
    }
    
    void add_transitions(const std::vector<T>& items)
    {
      auto N = static_cast<int>(items.size());
      if (N >= 2)
      {
        for (int i = 0; i < N - 1; ++i)
        {
          T from = items[i];
          T to = items[i+1];
          add_transition(from, to);
        }
      }
      if (N > 0)
        add_transition(items[N-1], empty_item);
    }
    
    int import_transitions(const std::string& filename)
    {
      std::ifstream input(filename);
      if (input.fail())
      {
        std::cerr << "Error opening file \"" + filename + "\"!" << std::endl;
        return EXIT_FAILURE;
      }
      std::stringstream ss;
      ss << input.rdbuf();
      input.close();
      
      std::string line;
      while (std::getline(ss, line))
      {
        // Break line into words.
        std::istringstream iss(line);
        std::vector<std::string> word_vec;
        std::string word;
        while (iss >> word)
          word_vec.emplace_back(word);
        
        add_transitions(word_vec);
      }
      
      return EXIT_SUCCESS;
    }
    
    void print() const
    {
      for (const auto& state : mctt)
      {
        std::cout << " [" << state.first << "] :" << std::endl;
        for (const auto& trg : state.second)
          std::cout << "   => " << trg.first << " : " << trg.second << std::endl;
      }
    }
  
    void normalize_transition_weights()
    {
      for (auto& targets : mctt)
      {
        int tot = 0;
        for (const auto& trg : targets.second)
          tot += trg.second;
        if (tot > 0)
        {
          for (auto& trg : targets.second)
            trg.second /= tot;
        }
        std::sort(targets.second.begin(), targets.second.end(),
                  [](const auto& A, const auto& B) { return A.second < B.second; });
      }
    }
    
    T generate(int min_num_items = -1, int max_num_items = -1) const
    {
      auto N = mctt.size();
      if (N == 0)
        return empty_item;
      
      size_t num_items = 1;
      T ret = empty_item;
      do
      {
        num_items = 1;
        auto rnd_idx = rnd::rand_int(0, static_cast<int>(N) - 1);
        int ctr = 0;
        const T* start_item_ptr = nullptr;
        for (auto& item : mctt)
        {
          if (rnd_idx == ctr++)
          {
            start_item_ptr = &item.first;
            break;
          }
        }
        if (start_item_ptr == nullptr)
          return empty_item;
        
        T item = *start_item_ptr;
        
        ret = item;
        while (item != "")
        {
          auto t = rnd::rand();
          const auto& entry = mctt.at(item);
          for (const auto& trg : entry)
          {
            if (t < trg.second)
            {
              item = trg.first;
              ret += item;
              if (item != "")
                num_items++;
              break;
            }
            t -= trg.second;
          }
        }
      } while ((min_num_items != -1 && num_items < min_num_items) || (max_num_items != -1 && num_items > max_num_items));

      
      return ret;
    }
  };

}
