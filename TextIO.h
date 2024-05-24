//
//  TextIO.h
//  Core
//
//  Created by Rasmus Anthin on 2024-05-24.
//

#pragma once

#include <fstream>
#include <iostream>
#include <string> 

namespace TextIO
{

  bool read_file(const std::string& file_path, std::vector<std::string>& lines)
  {
    std::ifstream file(file_path);
    
    if (!file.is_open())
    {
      std::cerr << "Error opening file \"" << file_path << "\"!" << std::endl;
      return false;
    }
    
    std::string l;
    
    while (std::getline(file, l))
      lines.emplace_back(l);

    file.close();
    
    return true;
  }
  
  bool write_file(const std::string& file_path, const std::vector<std::string>& lines)
  {
    std::ofstream file(file_path);
    
    if (!file.is_open())
    {
      std::cerr << "Error opening file \"" << file_path << "\"!" << std::endl;
      return false;
    }
    
    for (const auto& l : lines)
      file << l;
    
    file.close();
    
    return true;
  }

}
