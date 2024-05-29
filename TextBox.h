//
//  TextBox.h
//  Core
//
//  Created by Rasmus Anthin on 2022-11-22.
//

#pragma once
#include <vector>
#include <string>
#include <iostream>


struct TextBox
{
  std::vector<std::string> text_lines;
  
  TextBox(size_t N) : text_lines(N) {}
  
  std::string& operator[](size_t r_idx) { return text_lines[r_idx]; }
  
  void print() const
  {
    for (const auto& tl : text_lines)
      std::cout << tl << '\n';
  }
};
