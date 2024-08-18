//
//  StringBox.h
//  Core
//
//  Created by Rasmus Anthin on 2022-11-22.
//

#pragma once
#include "StringHelper.h"
#include <vector>
#include <string>
#include <iostream>


namespace str
{
  
  struct StringBox
  {
    std::vector<std::string> text_lines;
    
    StringBox() = default;
    StringBox(size_t N) : text_lines(N) {}
    StringBox(const std::vector<std::string>& texts) : text_lines(texts) {}
    StringBox(const std::string& text)
    {
      text_lines = tokenize(text, { '\n' });
    }
    
    std::string& operator[](size_t r_idx) { return text_lines[r_idx]; }
    
    void print() const
    {
      for (const auto& tl : text_lines)
        std::cout << tl << '\n';
    }
    
    size_t size() const { return text_lines.size(); }
    
    bool empty() const noexcept { return text_lines.empty(); }
  };

}
